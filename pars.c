#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define MAXRES 256
int nres= 0; char *res[MAXRES]= {0}, *pgen= NULL, *R[128]= {0};

int eor(char* r) { return !r || !*r || *r=='\n' || *r=='|' && r[-1]!='\\'; }

char* parseR(char r, char* s, int n); // FORWARD
char* pR(char* r, char* s, int n) { nres++; return parseR(*r, s, n); }
  

// return new string with S followed by A, free(s)
char* add(char* s, char* a, int n) { char* r= malloc((s?strlen(s):0)+n+1);
  strncat(strcpy(r, s?s:""), a?a:"", n); free(s); return r; }

// --- rules
// is a single Capital follows by =
//
//   D=0|1|2|3|4|5|6|7|8|9
//   A=a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
//   W=A|D
//   N=A*W
//   D=%d
//   X=fib\(+D\)
//   L=D,L|D     -- right recursion ok
//
// Undefined rule always matches...
// (space is ignored during matching)
// (no newlines)

// --- catching & generating output
//   L=D,L [$1 $2 +] | D
//
//   "1,2,3"   =>   "1 2 3 + +"

// --- char classes
//   %a = match alphabet and _
//   %d = match digit
//   %e = match end of text
//   %w = match word char (alphanum _)
//   %i = integer
//   %n = identifier (%a%w...)

// --- multiple matches PREFIX!
//   ?R = optionall match rule R
//   +R = match rule R once or more
//   *R = match rule R 0 or more times
//   (R must be a NonTerminal)
//
// TODO: +R redundant == R*R
// TODO: ?R redundant == S=R| and ?S
// TODO: *R redundant == S=RS|R|
//
// n==-1 infinity
// n==1 one time (for ? + *)
// Returns:
///  NULL=fail
//   rest of string (unparsed) it's "" if done.
#define Z goto next; case
char* parse(char* r, char* s, int n, int nr) { char*p= NULL,*os=s,*t; int on=n,onr=nres,x;
  DEBUG(if (debug>1) printf("    parse '%s' '%s' %d\n", r, s, n));
 next: while(n-- && r && s) { DEBUG(if (debug>2)printf("     next '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n))// TODO: join w prev line
switch(*r){ case 0: case'\n': case'\r': case'|': return s;
case' ':case'\t':while(isspace(*s))s++; r++;
Z '(':; Z '{':; // TODO: implement
Z '[': { DEBUG(if (debug>1) printf("BEFORE  RES='%s'\n", res[nres]));
  while(*++r!=']') { switch(*r){
    case '$': x=*++r-'0'+nr+1; res[nr]= add(res[nr], res[x], 999999); break;
    default: res[nr]= add(res[nr], r, 1);
    } DEBUG(if (debug>1) printf("  res[%d]='%s'\n", nr, res[nr]));
  } r++; goto next;
}
case'?':case'+':p=s;s=pR(r+1,s,1);if(*r=='?'){r+=2;s=s?s:p;goto next;}p=0;
case'*': r++; while(s&&*s)p=s,s=pR(r,s,1); r++,s=p;
#define X(x) ||strchr(x,*r)&&
Z'%':if(*++r=='e'&&!*s){r++;goto next;}p=s;do if((x=(*r=='_'X("anw")isalpha(*s)X(p
==s?"di":"dwin")isdigit(*s))))s++;else if(p==s)goto fail;while(0 X("in")x);r++;

Z 'A'...'Z': if ((p=pR(r++, s, -1))) s= p; else goto fail;
Z '\\': r++; default: if (*s==*r++) s++; /* matched */ else fail: {
  while(*r && !eor(r++)){}; if (eor(r)) return NULL; s=os;n=on;nres=onr;}
}}return s;}

#include <signal.h>

/// capture
char* parseR(char r, char* s, int n){
  DEBUG(if (debug>2) printf("    parseR '%c' (%d)\n", r, r));
  if (!r) raise(SIGTRAP); // TODO: ?
  int nr=++nres; free(res[nr]); res[nr]=0;
  char *x= parse(R[r],s,n,nr);
  // TODO: combine repeats? concat && nres-- after call parseR
  // TODO: matching with ? seems to "fail" ???
  if (!res[nr]) res[nr]=x?strndup(s, x-s):x; DEBUG(if (debug>1) printf("  ->%c.res[%d]='%s'\n", r, nr, res[nr]));
  nres= nr-1; return x;
}

// ENDWCOUNT

char* test(char rule, char* s) {
  nres= 0;
  char* e= parseR(rule, s, -1);
  DEBUG(printf("  %%%s %c-> '%s' RES=>'%s'\n\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e, res[nres+1]));
  if (e && !debug) printf("%s\n", res[nres+1]);
  return e;
}

//case '?': case '+': p=s;s=parse(R[*(r+1)],s,1);if(*r=='?'){r+=2;s=s?s:p;break;}if(
//!s)return s;else s=s?s:p;case '*': r++;while(s&&*s)p=s,s=parse(R[*r],s,1);r++,s=p;
    
//  Z '?': r++; p=s; s=parse(R[*r],s,1); r++; s=s?s:p;
//  Z '*': r++; do{p=s; s=parse(R[*r],s,1);}while(s && *s);r++;s=p;
//  Z '+': r++; s=parse(R[*r],s,1); while(s&&*s){p=s;s=parse(R[*r],s,1);};r++;s=p;


// TODO: count loader/reader?

// # comment
// A=foo|bar|X
// ?A
// foo
// barf
// @
// A=fie fum
// foo
// fie
// *A
// fie
// fum
void readparser(FILE* f) { char rule, *ln= NULL; size_t z= 0, d='\n';
  while(getdelim(&ln, &z, d, f)>0) {
    if (ln && ln[strlen(ln)-1]=='\n') ln[strlen(ln)-1]= 0;
    DEBUG(printf("%s\n", ln))
    if (!ln) break; if (!*ln) continue;
    if (ln[1]=='=' && isalpha(*ln) && isupper(*ln)) R[ln[0]]= strdup(ln+2);
    else if (*ln=='#') ; else if (*ln=='?') rule=ln[1];
    else if (*ln=='@') for(int i=0; i<127; i++) { free(R[i]); R[i]=NULL; }
    else if (*ln=='*') { rule=ln[1]; d=0; }
    //    else if (*ln=='$' && strchr("[{(<", ln[1])) { parsgen(ln+1, "", NULL); free(pgen); pgen= NULL; }
    else test(rule, ln);
  }
  free(ln);
}

// ENDWCOUNT

int main(int argn, char** argv) {
  do{ printf("ARG %s\n", *argv);
    if (0==strcmp("-d", argv[0])) debug++;
    argv++; } while(--argn);
  printf("debug=%d\n", debug);

  R['d']="%d";R['a']="%a";R['w']="%w";R['i']="%i";R['n']="%d";

 readparser(stdin);
}

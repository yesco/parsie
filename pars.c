#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 3; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define MAXRES 256
int nres= 0; char *res[MAXRES]= {0}, *pgen= NULL, *R[128]= {0};

int eor(char* r) { return !r || !*r || *r=='\n' || *r=='|'; }

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
char* parse(char* r, char* s, int n) { char*p= NULL,*os=s,*t; int on=n,x;
  DEBUG(if (debug>1) printf("    parse '%s' '%s' %d\n", r, s, n));
 next: while(n-- && r && s) { DEBUG(if (debug>2)printf("\tnext: '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n))// TODO: join w prev line
switch(*r){ case 0: case'\n': case'\r': case'|': return s;
Z '(':; Z '{':; Z '[':; // TODO: implement
case'?':case'+': p=s;s=parse(R[*(r+1)],s,1);if(*r=='?'){r+=2;s=s?s:p;break;}p=0;
case'*': r++; while(s&&*s)p=s,s=parse(R[*r],s,1); r++,s=p;
#define X(x) ||strchr(x,*r)&&
Z'%':if(*++r=='e'&&!*s){r++;break;}p=s;do if((x=(*r=='_'X("anw")isalpha(*s)X(p
==s?"di":"dwin")isdigit(*s))))s++;else if(p==s)goto fail;while(0 X("in")x);r++;
Z 'A'...'Z': if ((p=parse(R[*r++], s, -1))) s= p; else return p;
Z '\\': r++; default: if (*s==*r++) s++; /* matched */ else fail: {
  while(*r && !eor(r++)){}; if (eor(r)) return NULL; s=os; n=on; }
  if (!*s && eor(r)) return s;           }}return s;}

// ENDWCOUNT

char* test(char rule, char* s) {
  char *r= R[rule], *e= parse(r, s, -1);
  DEBUG(if (debug) printf("  %%%s %c-> '%s'\n\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e));
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

int main(void) {
R['d']="%d";R['a']="%a";R['w']="%w";R['i']="%i";R['n']="%d"; readparser(stdin);
}

//80----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

// Results and Attributes
#define NV 256
int nv= 0;
char *V[NV]= {0}, *R[128]= {0}, *A[NV]= {0};

// TODO: nv++ and nr? hmmm
void newV(int nr) { nv++; free(V[nr]); V[nr]=0; free(A[nr]); A[nr]= 0; }

int eor(char* r) { return !r || !*r || *r=='\n' || *r=='|' && r[-1]!='\\'; }

// find start of attr val string
char* attrval(int nr, char a) { for(int i=nr+1;i<NV;i++){ char* s= A[i];
  // TODO: not MAXRES...
  DEBUG(if (debug) if (s) printf("find.A[%d] = '%s'\n", i, s));
  while(s&&*s&&(s=strchr(s, ' ')))if(s[1]==a&&s[2]=='=') return s+3; else s++;
  }
  return 0;
}

// Using named Rule, parse String do only N matches
char* parseR(char r, char* s, int n); // FORWARD
char* pR(char* r, char* s, int n) { nv++; return parseR(*r, s, n); }

//#include "str.c"
// TODO: remove
char* sncat(char* s, char* x, int n) { int i= s?strlen(s):0, l= x?strlen(x):0; // DEBUG
  if (n<0 || n>l) n= l; s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0; // DEBUG
  return strncpy(s+i, x?x:"", n), s; // DEBUG
}


// Generate (add to *G) from [Rule] stop at ENDchar nr being $0 V[NR]
int gen(char** g, char* r, char end, int nr) { int n, l; char *or=r, *v, *e;
  DEBUG(if (debug>3) printf("GEN: '%s'\n", r));
  while(*r && *++r!=end && *r) { switch(*r){
    case '$': r++; if (isdigit(*r)) { v= V[*r-'0'+nr+1]; l=-1; } else if (isalpha(*r)) {
	DEBUG(if (debug) printf("ATTRVAL= '%s'\n", r))
	e= v= attrval(nr, *r); if (!e) break; while(*e && *++e && *e!=' '){}; l= e-v;
      } else if ((e=strchr("\"\"''(){}[]<>", *r)) && isdigit(*++r)) { // $"1 quoted
	v= V[*r-'0'+nr+1]; *g= sncat(*g, e+0, 1);
	printf("GEN e=%s r=%s v=%s \n", e, r, v); // DEBUG
	while(*v) { if (*v==e[1]) *g=sncat(*g, "\\", 1); *g= sncat(*g, v++, 1); }
	*g= sncat(*g, e+1, 1); break;
      }
      *g= sncat(*g, v, l); break;
    case '\\': r++; default: *g= sncat(*g, r, 1);
    }
    DEBUG(if (debug>2) printf("  V[%d]='%s'\n", nr, *g));
  }
  return r-or;
}

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
// (default result is verbatim input)
//
//   L=D,L [$1 $2 +] | D
//
//   "1,2,3"   =>   "1 2 3 + +"
//   A= int %n = %i; [_$1 $2 `$1] :t=int
//
//      $"1 $'2 $<3 $(4 $[5 $<6 - quote N

// --- char classes
//   %a = match alphabet and _
//   %d = match digit
//   %e = match end of text
//   %w = match word char (alphanum _)
//   %i = integer
//   %n = identifier (%a%w...)
//
// -- string matcher
//     ($1 always gives unquoted)
//     (default is verbatim)
//
//   %" = "foo\"bar"
//   %' = 'c' or 'foo\'bar'
//     (not handling nesting:???)
//   %( = ( fo( oooo ( \) foo bar)
//   %[ = [ fo[ oooo [ \] foo bar]
//   %{ = { fo{ oooo { \} foo bar}
//   %< = < fo< oooo < \> foo bar>
//   %s = START fo? START \END foo bar END
//     handle any style string


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
//

#define Z goto next; case 

// Parse a Rule for String using max N matches starting at results at NR
//   n==-1 infinity
//   n==1 one time (for ? + *)
// Returns:
///  NULL=fail
//   rest of string (unparsed)
char* parse(char* r,char* s,int n,int nr){char*p=0,*os=s,*t,*m;int on=n,onr=nv,x;
  DEBUG(if (debug>2) printf("    parse '%s' '%s' %d\n", r, s, n));
  // TODO: move while below next?
next: while(n-- && r && s) { while(isspace(*s)) s++;
DEBUG(if (debug>3)printf("     next '%s' (%d)\n\t   of '%s' left=%d\n",r,*r,s,n));

switch(*r){
case 0: case '\n': case '\r': case '|': return s; // end
Z' ': case '\t': while(isspace(*s))s++; r++;

Z'(': Z'{': assert(!"TODO: implement");
Z'[': r+= 1+gen(V+nr, r, ']', nr);
Z':': A[nr]= sncat(A[nr], " ", 1); r+= 1+gen(A+nr, r, ' ', nr);

// TODO: too clever to reuse instead of clear...
case '?': case '+': case '*': newV(nr);
  DEBUG(if (debug>1) printf("== res %d\n", nr)); // DEBUG
  // TODO: is nv right? "passthrough doesn't seem to work for $s %" etc..."
  // TODO: several places?
  switch(*r){
  case'?': case '+': p=s; s=parse(R[r[1]],s,-1,nr);
    if (*r=='?') { r+=2; s=s?s:p;goto next; } else if (!s) goto fail;
    // fallthrough
  case'*': r++; while(s&&*s) { p=s; s=parse(R[*r],s,-1,nr); } r++; s=s?s:p;
  }
//  Z '?': r++; p=s; s=parse(R[*r],s,1); r++; s=s?s:p;
//  Z '*': r++; do{p=s; s=parse(R[*r],s,1);}while(s && *s);r++;s=p;
//  Z '+': r++; s=parse(R[*r],s,1); while(s&&*s){p=s;s=parse(R[*r],s,1);};r++;s=p;

// TODO: also too clever encoding?
Z'%': ++r; if ((p=strchr("\"\"''(){}[]<>", *r=='s'?*s:*r))) {
  if (*s!=*p) goto fail; else newV(nr);
  // move till unquoted endchar
  while(*++s&&*s!=p[1]) { if (*s=='\\') s++; V[nv+nr]=sncat(V[nv+nr], s, 1); }
  r++; s++; goto next;
}
  // char class: %a %d %w %i %n %e
  // The T string classify curent
  // char, returning list of classes
  // "XYZZZZ" X skipped if first ch
  // XY will take more chars
  p=s; do{ t=isdigit(*s)?"nidw":isalpha(*s)?" nwa":*s=='_'?" nw":*s?0:"  e";
    m= t?strchr(t+(s==p), *r):0; if (!m && s==p) goto fail;
  } while(*++s && m && m-t<2);  s-=!m; r++;

Z'A'...'Z': if ((p=pR(r++, s, -1))) s= p; else goto fail;
Z'\\': r++; default: if (*s==*r++) s++; /* matched */ else fail: {
  DEBUG(if (debug>2) printf("     FAIL '%s' '%s'\n", r-1, s));
  while(*r && !eor(r++)){}; if (eor(r) && r[-1]!='|') return NULL;
  // restore start state
  s=os; n=on; nv=onr;
  DEBUG(if (debug>2) printf("     |OR  '%s' '%s'\n", r-1, s));
  } // fail
} // switch
} // while
  return s;
}

/// capture
char* parseR(char r, char* s, int n) { int nr=++nv;
  DEBUG(if (debug>1) printf("  parseR '%c' (%d)\n", r, r));
  nv--; newV(nr);
  char *x= parse(R[r],s,n,nr);
  // TODO: combine repeats? concat && nv-- after call parseR
  // TODO: matching with ? seems to "fail" ???
  // TODO: for matching only subrule "| X" default should be [$1] ???
  if (!V[nr]) V[nr]=x?strndup(s, x-s):x;
  DEBUG(if (debug>1) printf("  ->%c.V[%d]='%s'\n", r, nr, V[nr]));
  nv= nr-1; return x;
}

char* test(char rule, char* s) { nv= 0;
  char* e= parseR(rule, s, -1);
  if (!e || *e) printf("%%%s %c-> '%s' RES=>'%s'\n\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e, V[nv+1]);
  printf("%s\n", V[nv+1]); return e;
}

void readparser(FILE* f) { char rule, *ln= NULL; size_t z= 0, d='\n';
  while(getdelim(&ln, &z, d, f)>0) {
    if (ln && ln[strlen(ln)-1]=='\n') ln[strlen(ln)-1]= 0;
    DEBUG(printf("> %s\n", ln))
    if (!ln) break;
    if (!*ln) continue;
    if (ln[1]=='=' && isalpha(*ln) && isupper(*ln)) R[ln[0]]= strdup(ln+2);
    else if (*ln=='#') ; // comment
    else if (*ln=='?') rule=ln[1];
    else if (*ln=='*') { rule=ln[1]; d=0; }
    //    else if (*ln=='$' && strchr("[{(<", ln[1])) { parsgen(ln+1, "", NULL); free(pgen); pgen= NULL; }
    else test(rule, ln);
  }
  free(ln);
}

// ENDWCOUNT

int main(int argc, char** argv) {
  assert(sizeof(long)==8);
  assert(sizeof(void*)==sizeof(double));
  assert(sizeof(void*)==sizeof(long));
  assert(sizeof(void*)==sizeof(long));

  // parse arguments
  do{
    if (0==strcmp("-d", argv++[0])) debug++;
  } while(--argc);

  // init shortnames
  for(char*s="dawin\"'({[<"; *s; s++) {
    char x[]={'%',*s,0};
    R[*s]=strdup(x);
  }

  readparser(stdin);
}

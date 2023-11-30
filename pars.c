#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 1; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define SL sizeof(long)

#define MAXRES 256
int nres= 0; char *res[MAXRES]= {0}, *pgen= NULL;

// NonTerminal RuleName-> def
char* R[128]= {0};

int eor(char* r) { return !r || !*r || *r=='\n' || *r=='|'; }

//#define Z ;if(!s)return s; else goto next; case
#define Z goto next; case

// Rules:
//   D=0|1|2|3|4|5|6|7|8|9
//   A=a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
//   W=A|D
//   N=A*W
//   D=%d
//   X=fib\(+D\)
//
// Undefined rule always matches...
//
// --- multiple matches PREFIX!
// ?R = optionall match rule R
// +R = match rule R once or more
// *R = match rule R 0 or more times
// (R must be NonTerminal, no %a %d %w)
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
char* parse(char* r, char* s, int n) { char*p= NULL;
  DEBUG(printf("    parse '%s' '%s' %d\n", r, s, n));
 next: while(n-- && r && s) { DEBUG(printf("\tnext: '%s' (%d)\n\t   of '%s' left=%d\n", r, *r, s, n))
  switch(*r) {
  case 0: case'\n': case'\r': case'|': return s;
    Z '(':; Z '{':; Z '[':;
    Z '?': p=s; s=parse(R[*++r],s,1); r++; s=s?s:p;
    Z '*': r++; do{ p=s; s=parse(R[*r],s,1); }while(s && *s); r++; s=p;
    Z '+': r++; s= parse(R[*r],s,1); while(s && *s){ p=s; s=parse(R[*r],s,1); }; r++; s=p;
    Z '%': switch(*++r){ // TODO: too long...
      Z 'a': if (isalpha(*s)||*s=='_') r++,s++; else return NULL;
      Z 'd': if (isdigit(*s)) r++,s++; else return NULL;
      Z 'w': if (isalnum(*s)||*s=='_') r++,s++; else return NULL;
    };
    Z 'A'...'Z': if ((p=parse(R[*r++], s, -1))) s= p; else return p;
    Z '\\': r++;
  default: if (*s==*r++) s++; else { while(*r && !eor(r++)){} n++; // TODO: should restore N?
      if (eor(r)) return NULL; } if (!*s && eor(r)) return s;
  }
  } // DEBUG
  return s;
}

char* test(char rule, char* s) {
  char *r= R[rule], *e= parse(r, s, -1);
  printf("  %%%s %c-> '%s'\n\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e); // DEBUG
  return e;
}

// TODO: count loader/reader?
// ENDWCOUNT

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
  // save 4 lines of source?
  R['d']= "0|1|2|3|4|5|6|7|8|9"; // digit
  R['a']= "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_"; // letter
  R['w']= "?a?d"; // "word" char
  R['i']= "+a*w"; // identifier
//   W=A|D
//   N=A*W
  readparser(stdin);
}

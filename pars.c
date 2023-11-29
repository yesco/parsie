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

#define Z ;goto again; case

char* parse(char* r, char* s) {
 again: while(1) { DEBUG(printf("\tPARSE: '%s' of '%s'\n", r, s))
  switch(*r) {
  Z 0: Z '\n': Z '\r': Z '|': return s;
  Z '(': Z '{': Z '[':
  Z '?': Z '*': Z '+':
  Z '$':
  Z '\\': r++;
  default: if (*s==*r++) s++; else while(*r && *r++!='|');
  }
  } // DEBUG
}

char* test(char rule, char* s) {
  char *r= R[rule], *e= parse(r, s);
  printf("%%%s %c-> '%s'\n", e?(*e?"UNPARSED":"MATCHED!"):"FAILED", rule, e); // DEBUG
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
  readparser(stdin);
}

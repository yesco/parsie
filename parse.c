#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 1;

// char==name of NonTerminals
char* NT[128]= {0};

int eor(char* r) {
  return !*r || *r=='\n' || *r=='|';
}
 
char* parset_hlp(char target, char* s);
char* parset(char target, char* s);

// match a char or a NT with S
char* match(char r, char* s) {
  // one space matches zero or more
  //if (r==' ') { while(isspace(*s)) s++; return s; }
  // spaceis always removed
  if (debug>=3) printf("\n\tMATCH '%c' %d '%s'\n", r, r, s);
  while(isspace(*s)) s++;
  if (r=='a'+128) return isalpha(*s) || *s=='_' ? s+1 : NULL;
  if (r=='d'+128) return isdigit(*s) ? s+1 : NULL;
  if (r=='w'+128) return isalnum(*s) || *s=='_' ? s+1 : NULL;
  if (r==*s) return s+1;
  if (isupper(r)) return parset(r, s);
  return NULL;
}
  
#define MAXRES 256
int nres= 0;
char* res[MAXRES]= {0};
char* pgen= NULL;

char* parset_gen(char* r, char* s, char* p) {
  // TODO: use dstrprintf?
  char buf[16*1024]= {0};
  if (debug>=1) printf("\n\t>>>>>\n");
  while(*r && *r++!=']') {
    if (*r=='$') {
      char n= *++r;
      // TODO: offset in res
      if (isdigit(n)) strcat(buf, res[n-'0']);
      else if (n=='$') strncat(buf, s, p-s);
    } else strncat(buf, r, 1);
  }
  if (debug>=1) printf("\t%s\n<<<<<<\n", buf);
  // TODO: offset in res
  pgen= strdup(buf);
  return p;
}

// parse a term
// extract term match
char* parset(char target, char* s) {
  int n= nres++;
  pgen= NULL;
  char* r= parset_hlp(target, s);
  nres--;
  if (r==s) return NULL; // failed
  // sub rule matched - store
  assert(n<MAXRES);
  free(res[n]);
  // TODO: keep pointer+len???
  res[n]= pgen?pgen:strndup(s, r-s);
  if (debug) printf("\t'%c' ==> (%d)= '%s'\n", target, n, res[n]);
  nres= n+1;
  return r;
}

// match S to TARGET rule
// null if not match
// otherwise remaining string
char* parset_hlp(char target, char* s) {
  char* r= NT[target];
  int ok= 0;
  char* p= s;
  if (debug>1) printf("\t'%s'\t'%s", p, r);
  while(!eor(r)) {
    // space is always removed?
    // TODO: end?
    while(isspace(*r)) r++;
    while(isspace(*s)) s++;
    // action == accept, gen out
    if (*r=='[') return parset_gen(r, s, p);
    //if (*r=='{') return eval(r, s, p); // TODO:
    // $, $; - delimited-list
    int min=1, max=1, delim= 0;
    char c= *r;
    if (*r=='$') {
      c= *++r;
      delim= strchr(",; ", c)?c:0;
      min= (c=='?'||c=='*'||delim)?0:1;
      max= (c=='+'||c=='*'||delim)?9999:1;
      if (strchr("?*+,; ", c)) c= *++r;
      switch(c) {
      case 'a': case 'd': case 'w': c=c+128; break;
      case '/': c=*++r; break; // TODO: rest is optional, how about gen?
      }
    }
    if (*r=='\\') r++; // quoted char

    char* m, *last= NULL;
    // TODO: concat captures?
    while(max-- && (m=match(c, p))) {
      if (*m && *m==delim) m++;
      last= p= m;
    }

    if (min && !last) {
      // failed: look for alt
      while(*r && *r++!='|');
      ok= 0, p= s;
      if (debug>1) printf("\n\tFAILED\n");
    } else ok= 1, r++; //, p= m, 
    if (debug>=2) printf("\t'%s'\t%s", p, r);
  }
  return (ok && eor(r)) ? p : 0;
}

char* parse(char target, char* s) {
  for(int i=0; i<MAXRES; i++)
    if (res[i]) {
      free(res[i]);
      res[i]= NULL;
    }
  nres= 0;
  return parset(target, s);
}
  
void test(char target, char* s) {
  char* p= parse(target, s);

  printf("\tTEST '%s' -> '%s'\n", s, p);
  for(int i=0; i<nres; i++)
    printf("\t[%d]= '%s'\n", i, res[i]);
  printf("\t======================\n");
  // print previous
  for(int i=nres; i<256; i++)
    if (res[i]) printf("\t  [%d]= '%s'\n", i, res[i]);
  printf("\n");
}

void readparser(FILE* f) {
  char rule, *ln= NULL; size_t z= 0;
  while(getline(&ln, &z, f)>0){
    if (ln && ln[strlen(ln)-1]=='\n') ln[strlen(ln)-1]= 0;
    if (debug) printf("%s\n", ln);
    if (!ln) break; if (!*ln) continue;
    if (ln[1]=='=' && isalpha(*ln) && isupper(*ln)) NT[ln[0]]= strdup(ln+2);
    else if (*ln=='#') ; // comment
    else if (*ln=='?') rule=ln[1];
    else test(rule, ln);
  }
  free(ln);
}

int main(void) {
  readparser(stdin);
}

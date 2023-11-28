#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int debug= 1; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#define SL sizeof(long)

// simple dictionary
#define DICTSIZE 16*1024
char dict[DICTSIZE]= {0};
// set/restore to do dynamic scoping!
int dix= 0;

// register atom, set val first time
long atom(char* s, long val) { long i= 0;
  while(i<dix){if(0==strcmp(s,dict+i))return i;i+=strlen(dict+i)+1+SL;}
  strcpy(dict+dix,s);dix+=strlen(s)+1;memcpy(dict+dix,&val,SL);dix+=SL;return i;
}

// get value for atom
long atomval(long i) {long r;memcpy(&r,dict+i+strlen(dict+i)+1,SL);return r;}

// char==name of NonTerminals
char* NT[128]= {0};

int eor(char* r) { return !r || !*r || *r=='\n' || *r=='|'; }
 
char* parset_hlp(char target, char* s); // FORWARD
char* parset(char target, char* s); // FORWARD

// match a char or a NT with S
// $a=alpha $d=digit $w=word $e=end
char* match(char r, char* s) {
  DEBUG(if (debug>=3) printf("\n\tMATCH '%c' %d '%s'\n", r, r, s))
  // spaceis always removed (TODO remove?)
  while(isspace(*s)) s++;
  if (r=='a'+128) return isalpha(*s) || *s=='_' ? s+1 : NULL;
  if (r=='d'+128) return isdigit(*s) ? s+1 : NULL;
  if (r=='w'+128) return isalnum(*s) || *s=='_' ? s+1 : NULL;
  if (r=='e'+128) return !*s ? s : NULL;;
  if (r==*s) return s+1;
  return r==*s?s+1 : isupper(r)?parset(r, s) : NULL;
}
  
#define MAXRES 256
int nres= 0;
char *res[MAXRES]= {0}, *pgen= NULL;

char* parsgen(char* r, char* s, char* p) { char buf[16*1024]= {0}; // unsafe
  DEBUG(if (debug>=1) printf("\n\tGEN>>>>>\n"))
  while(*r && *r!=']') if (*r=='$') {
    char n= *++r; r++;
    // TODO: offset in res
    if (isdigit(n)) {
      printf("GEN: take $%c '%s'\n", n, res[n-'0']); // DEBUG
      printf("  :  buf=%s\n", buf); /// DEBUG
      if (res[n-'0']) strcat(buf, res[n-'0']);
      printf("  => buf=%s\n", buf); // DEBUG
    } else if (n=='$') strncat(buf, s, p-s); } else strncat(buf, r++, 1);
  DEBUG(if (debug>=1) printf("\t%s\n<<<<<<\n", buf))
  // TODO: offset in res
  pgen= strdup(buf); return p;
}

// parse a term
// extract term match
char* parset(char target, char* s) {
  int n= nres++; pgen= NULL; char* r= parset_hlp(target, s); nres--;
  if (r==s) return NULL; // failed
  // sub rule matched - store
  assert(n<MAXRES);
  // TODO: keep pointer+len???
  free(res[n]); res[n]= pgen?pgen : strndup(s, r-s);
  DEBUG(printf("\t'%c' ==> (%d)= '%s'\n", target, n, res[n]))
  nres= n+1; return r;
}

// match TARGET rule to string S
//   NULL if not match
//   otherwise remaining string
char* parset_hlp(char target, char* s) {
  char *r= NT[target], *p= s; int ok= 0;
  DEBUG(if (debug>1) printf("\t'%s'\t'%s", p, r))
  while(!eor(r)) {
    // space is always removed? TODO: don't
    while(isspace(*r)) r++; while(isspace(*s)) s++;
    // action == accept, gen out
    if (*r=='[') return parsgen(r+1, s, p);
    //if (*r=='{') return eval(r, s, p); // TODO:
    // $, $; - delimited-list
    int min=1, max=1, delim= 0; char c= *r;
    if (*r=='$') {
      c= *++r; delim= strchr(",; ", c)?c:0;
      min= (c=='?'||c=='*'||delim)?0:1;
      max= (c=='+'||c=='*'||delim)?9999:1;
      if (strchr("?*+,; ", c)) c= *++r;
      switch(c) {
      case 'a': case 'd': case 'w': case 'e': c=c+128; break;
      case '/': c=*++r; break; // TODO: rest is optional, how about gen?
      }
    }
    if (*r=='\\') r++; // quoted char

    char *m, *last= NULL;
    // TODO: concat captures?
    while(max-- && (m=match(c, p))) { if (*m && *m==delim) m++; last= p= m; }

    if (min && !last) {
      // failed: look for alt
      ok= 0; p= s; while(*r && *r++!='|');
      DEBUG(if (debug>1) printf("\n\tFAILED\n"))
    } else ok= 1, r++; //, p= m, 
    DEBUG(if (debug>=2) printf("\t'%s'\t%s", p, r))
  }
  return (ok && eor(r)) ? p : 0;
}

char* parse(char target, char* s) {
  for(int i=0; i<MAXRES; i++) { free(res[i]); res[i]= NULL; } nres= 0;
  return parset(target, s);
}
  
void test(char target, char* s) { char* p= parse(target, s);

  printf("\t%s rule -> '%s'\n", p?"UNPARSED":"FAILED", p); // DEBUG
  for(int i=0; i<nres; i++) printf("\tRESULT[%d]= '%s'\n", i, res[i]); // DEBUG
  printf("\t======================\n"); // DEBUG
  // print previous
  for(int i=nres; i<256; i++) if (res[i]) printf("\t  [%d]= '%s'\n", i, res[i]); // DEBUG
  printf("\n"); // DEBUG
}

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
    if (ln[1]=='=' && isalpha(*ln) && isupper(*ln)) NT[ln[0]]= strdup(ln+2);
    else if (*ln=='#') ; else if (*ln=='?') rule=ln[1];
    else if (*ln=='@') for(int i=0; i<127; i++) { free(NT[i]); NT[i]=NULL; }
    else if (*ln=='*') { rule=ln[1]; d=0; }
    else if (*ln=='$' && strchr("[{(<", ln[1])) {
      parsgen(ln+1, "", NULL);
      printf("GEN: %s\n", pgen); // DEBUG
      free(pgen); pgen= NULL;
    } else test(rule, ln);
  }
  free(ln);
}

// ENDWCOUNT

int main(void) {
  if (0) { // test dictionary
    long nl= atom("NULL", 0);
    long foo= atom("foo", 14);
    long bar= atom("bar", 42);
    printf("NULL=%ld foo=%ld bar=%ld\n", nl, foo, bar);
    nl= atomval(nl);
    foo= atomval(foo);
    bar= atomval(bar);
    printf("NULL=%ld foo=%ld bar=%ld\n", nl, foo, bar);
    exit(3);
  }

  readparser(stdin);
}

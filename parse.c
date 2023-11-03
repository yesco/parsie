#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

// char==name of NonTerminals
char* NT[128]= {0};

int nont(char t) {
  return t>='A' && t<='Z';
}

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
  while(isspace(*s)) s++;
  if (r==*s) return s+1;
  if (nont(r)) return parset(r, s);
  return NULL;
}
  
#define MAXRES 256
int nres= 0;
char* res[MAXRES]= {0};

// parse a term
// extract term match
char* parset(char target, char* s) {
  int n= nres++;
  char* r= parset_hlp(target, s);
  nres--;
  if (r==s) return NULL;
  // sub rule matched - store
  assert(n<MAXRES);
  free(res[n]);
  // TODO: keep pointer+len???
  res[n]= strndup(s, r-s);
  printf("  --(%d)= '%s'\n", n, res[n]);
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
  printf("  '%s'\t'%s", p, r);
  while(!eor(r)) {
    // space is always removed?
    // TODO: end?
    while(isspace(*r)) r++;
    while(isspace(*s)) s++;

    // action == accept, gen out
    if (*r=='[') {
      // TODO: use dstrprintf?
      char buf[1024]= {0};
      printf("\n>>>>>\n");
      while(*r && *r++!=']') {
	if (*r=='$') {
	  char n= *++r;
	  // TODO: offset in res
	  if (isdigit(n)) strcat(buf, res[n-'0']);
	  else if (n=='$') strncat(buf, s, p-s);
	} else strncat(buf, r, 1);
      }
      printf("%s\n<<<<<<\n", buf);
      // TODO: offset in res
      res[0]= strdup(buf);
      return p;
    }

    char* m= match(*r, p);
    if (!m) {
      // failed: look for alt
      while(*r && *r++!='|');
      p= s;
      ok= 0;
    } else {
      r++, p= m;
      ok= 1;
    }
    printf("  '%s'\t%s", p, r);
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

  printf("=== %s -> '%s'\n", s, p);
  for(int i=0; i<nres; i++)
    printf("  [%d]= '%s'\n", i, res[i]);
  printf("======================\n");
  // print previous
  for(int i=nres; i<256; i++)
    if (res[i]) printf("  [%d]= '%s'\n", i, res[i]);
  printf("\n");
}

int main(void) {
  // longer match first 33
  //NT['D']= "0|1|2|33|3|4|5|6|7|8|9\n";
  NT['D']= "0|1|2|3|4|5|6|7|8|9\n";
  NT['N']= "DN|D\n";
  NT['Q']= "select N, N from int(N, N) i\n";
  NT['Q']= "select N, N from int(N, N) i [ QUERY($$)\n COLS($1,$2)\n IOTA($3,$4) ]\n";
  
  test('D', "0");
  test('D', "1");
  test('D', "7");
  test('D', "3");
  test('D', "33");
  test('D', "44");
  test('D', "a");

  test('N', "42");
  test('N', "428");
  test('N', "4");
  test('N', "a428");

  test('Q', "select 2222,123456 from int(1, 10) i");
}



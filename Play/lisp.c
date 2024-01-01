#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef char* L;

//  >0 - chars taken
//  =0 - no args
//  <0 - number of args
// -10 - var args... TODO: ?
int SZ(L a) {
  if (isspace(*a)) return 1+SZ(a+1);
  switch(*a) {
  case 0: case 'T': case 'Y':
    return 1;
  case '_': case 'a'...'z': case '0'...'9': 
    return 1;
  case '.': { int b= SZ(a+1); int c= SZ(a+1+b); return 1+b+c; }
  case 'A': case 'D': case 'L': case 'P': case 'U': case 'V': case 'W': case '~': case '#': case '$': case '\'':
    return -1;
  case 'J': case 'K': case '\\':  case ':': case '"':  case '(':
    return -10;
  case 'I': case 'Z': case '?':
    return 3;
  default: return -2;
  }
} 
	
int LN(L a) {
  //printf("LN:%s\n", a);
  int n= SZ(a), t= 0;
  if (n>=0) return n;
  t++; a++;
  while(-n++) {
    //printf("%d.ITER:%s\n", n-1,a);
    int nn= LN(a);
    t+= nn; a+= nn;
  }
  return t;
}

L getval(L a, L e) { return "9"; }

L A(L c) { return strndup(c, LN(c)); }
L D(L c) { return c+LN(c); }
L C(L a, L d) { if (!a) a= ""; if (!d) d= "";
  // TODO: if car is a cons, then need parens?
  char s[strlen(a)+strlen(d)+1];
  strcpy(s, a); strcat(s, d);
  return strdup(s);
}
  
L E(L x, L e) {
  printf("E>%s<\n", x);
  if (!x || !*x) return x;
  switch(*x) {
  case ' ': return " "; // TODO: or ""?
  case 'a'...'z': return getval(x, e);
  case '0'...'9': return strndup(x, 1);
      // TODO: how to handle car being a inline lambda?
  }
  //L a= A(x); // TODO: eval if lambda? var?
  // non-eval args
  switch(*x){
  case 0: return x;
  case '.':  
  case 'I':
  case '!':
  case '\'':
  case '\\': return "8"; // TODO:;
  }
  // eval 1 arg
  L bb= A(x+1);
  L b= E(bb, e);
  switch(*x){
  case 'A': return A(b);
  case 'D': return D(b+SZ(x+1));
  }
  // eval 2 arg
  L c= E(A(D(x+1)), e);
  switch(*x){
  case 'C': return C(b, c);
  }
  // error
  return "8"; // TODO:
}

void test(char *s) {
  printf(">%s<\n", s);
  printf("\t=%d\n", LN(s));
  printf("\tA=%s\n", A(s));
  printf("\tD=%s\n", D(s));
  printf("\tE=%s\n", E(s, ""));
}

int main(void) {
  test("a");
  test("3");
  test("T");
  test("C3_");
  test(".ab");
  test("C1C2C3_");
  test("C1C+11C+1+11_");
  test("C34"); // dotted pair?
  test("CC12C34abc_"); // dotted pair?
  test("CAC12DC34abc_");
}

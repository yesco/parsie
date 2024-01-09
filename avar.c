// atomic Stack VARs
//
// For keeping track of stack state
// and global variables during
// parsing/compilation.

// int returned:
// -N: N/100 frames up, N%100 local num
// -n: means n:th in current frame <100
//  0: not found
// +n: means n:th D global value offset

#include <stdlib.h> // DEBUG
#include <stdio.h> // DEBUG
#include <string.h> // DEBUG
#include <strings.h> // DEBUG

#ifdef avarTEST // DEBUG
  #include "alf.c" // DEBUG
#endif // DEBUG

int frame=0, ln= 0;

int newvar(char* n) { D a; long ofs=0; if (frame) ofs= -((frame-1)*100 + ++ln);
  a= ofsatom(ofs, n); return a; }

void enterframe() { *--Y= ln; *--Y= atom("--frame--"); frame++;}

int findvar(char* n){D v=varatom(n);long t=atomofs(v);return t<0?t:atomaddr(v);}
  
int setvar(char* n) { return findvar(n); }

void exitframe() { D f= atom("--frame--"); frame--; ln=-10000;
  while(Y<=K+SMAX-2) if (deq(f, *Y)) { Y++; ln= *Y++; return; } else Y+=2; }

// ENDWCOUNT

#ifdef avarTEST

void f(char* n){
  int l= findvar(n);
  printf("%s=%5d  ", n, l);
}

void PP() {
  //printf("=="); pr(globals); printf("\n");
  //printf("--"); pr(vars); printf("\n");
  f("gfoo"); f("a"); f("gfish"); f("b");
  P("\n");
}

int main() {
  initmem(16*1024); inittypes();
  prK();
  
  //gn=1234566, ln=777776;
    
  PP();
  setvar("gfoo"); PP();
  newvar("a"); PP();
  enterframe(); {
    newvar("a"); PP();
    setvar("gfoo"); PP();
    setvar("gfish"); PP();
    newvar("b"); PP();
    enterframe(); {
      newvar("c"); PP();
      newvar("a"); PP();
    } exitframe();
  } exitframe();
  newvar("b"); PP();
  newvar("aaaa"); PP(); 
  //printf("atom: '%s'\n", dchars(atomix("foo")));
  //printf("=="); pr(globals); printf("\n");
  prK();
}

#endif

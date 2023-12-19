#include <stdlib.h> // DEBUG
#include <stdio.h> // DEBUG
#include <string.h> // DEBUG
#include <strings.h> // DEBUG

void pr(char* s) { while(s && *s) {  if (*s>=' ') putchar(*s++); else printf("=%d ", *s++);  } } // DEBUG


char globals[16*1024]={0}, vars[sizeof(globals)]={0}; int gn=15, ln=0;

void newframe() { ln=0; }

int newvar(char* vs, int* nn, char* n) { int l= strlen(n); memmove(vs+l+1, vs,
  sizeof(globals)-l-1); vs[l]= ++*nn; memmove(vs, n, l); return *nn; }

// might find substring...
int _findvar(char* vs, char* n) { char* s=vs; int l=strlen(n); while(s &&
  (s=memmem(s, strlen(s), n, l)) && s && *s) if (s[l]<32) return s[l]; else
    while(*s && *s>' ')s++; return 0; }

/// ooops only 31 globals
int findvar(char* n) { int i= _findvar(vars, n); return i?-i:_findvar(globals, n); }

int setvar(char* n) { int i= findvar(n); return i?i:newvar(globals, &gn, n);}

// TODO: restore?
void exitframe() { ln=-667; }

// ENDWCOUNT

void f(char* n){
  int l=findvar(n);
  printf("  %s=%d\n", n, l);
}

void P() {
  printf("=="); pr(globals); printf("\n");
  printf("--"); pr(vars); printf("\n");
  f("gfoo"); f("a"); f("gfish"); f("b");
}

int main() {
  P();
  setvar("gfoo"); P();
  newvar(vars, &ln, "a"); P();
  setvar("gfish"); P();
  newvar(vars, &ln, "b"); P();
}

       

// Stack VAR
//
// For keeping track of stack state
// and global variables during
// parsing/compilation.

// int returned:
// -n: means n:th in current frame
//  0: not found
// +n: means n:th 8B value (double)

#include <stdlib.h> // DEBUG
#include <stdio.h> // DEBUG
#include <string.h> // DEBUG
#include <strings.h> // DEBUG

// Reads a "Base 128 Varint"
// (same Google Protocol Buffers)
// High bit is continuation flag

int rdint(char**s){int i=0,l=0;while(**s>127)i+=(*(*s)++&127)<<l,l+=7;return i;}

void pr(char* s) { while(s && *s) if (*s<128) putchar(*s++); else printf("=%d ", rdint(&s)); } // DEBUG

//char globals[16*1024]={0}, vars[sizeof(globals)]={0}; int gn=0, ln=0;

char globals[16*1024]={0}, vars[sizeof(globals)]={0}; int gn=123455, ln=777776;

void newframe() { ln=0; }

int newvar(char* vs, int* nn, char* n) { int l= strlen(n); char s[l+16];
  memcpy(s, n, l); int i= ++*nn; while(i) {s[l++]= 0x80+(i&0x7f);i>>=7;}
  memmove(vs+l, vs, sizeof(globals)-l); memmove(vs, s, l); return *nn; }

// might find substring...
int _findvar(char* vs, char* n) { int l=strlen(n); while(*vs) {
    if (0==strncmp(vs, n, l) && *(vs+=l)>127) return rdint(&vs);
    while(*vs && *vs<128) vs++; rdint(&vs); } return 0; }

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
  newvar(vars, &ln, "aaaaaa"); P(); 
}

       

// Dynamic strings library
//
// A dynamic string is just a malloced char[]
// it however, can grow as needed.
// No book-keeping required! ?
// It grows in predetermined chunks.

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

char* snadd(char* s, char* x, int n) {
  int i= s?strlen(s):0;
  n= x?strlen(x):0;
  s= realloc(s, 1024*((i+n+1024)/1024)); s[i+n]= 0;
  return strncpy(s+i, x?x:"", n), s;
}

// ENDWCOUNT

char* p(char* s, char* x, int n) {
  char* r= snadd(s, x, n);
  if (s!=r) printf("REALLOC ");
  printf(" '%s' len=%lu\n", r, strlen(r));
  return r;
}

int main() {
  char* s= NULL;
  for(int i=0; i<1024*16; i++) {
    s= p(s, "a", 3);
  }
}

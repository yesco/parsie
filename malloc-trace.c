// print trace malloc
//
// Textual subititution
//
// To test one "file" include this in it

#include <dlfcn.h>
#include <assert.h>

#define XMMAX 1024
void* xP[XMMAX]={0}; char xS[XMMAX]= {0};
int xN= 1;
  
// Find pointer number
// 0 if not found
int xfind(void* p) {
  for(int i=0; i<xN; i++)
    if (xP[i]==p) return i;
  return 0;
}

void xlive() {
  printf("LIVE: %s\n", xS+1);

  printf("      ");
  for(int i=1; i<=strlen(xS+1); i++) {
    putchar(i%10+'0');
  }
  printf("\n");

  printf("     ");
  for(int i=1; i<=strlen(xS+1); i+=10) {
    printf("%-10d", i/10);
  }
  printf("\n");

}

void* xmalloc(size_t z) {
  static void* (*f)(size_t);
  if (!f) f= dlsym(RTLD_NEXT, "malloc");
  assert(f);
  void* p= f(z);

  xP[xN]= p; xS[xN]= 'M';
  assert(++xN <= XMMAX);

  printf("\nM : %2d %4zu %p\n", xN-1,z, p);
  xlive();
  return p;
}
#define malloc xmalloc

void* xcalloc(size_t memb, size_t n) {
  static void* (*f)(size_t, size_t);
  if (!f) f= dlsym(RTLD_NEXT, "calloc");
  assert(f);
  void* p= f(memb, n);

  xP[xN]= p; xS[xN]= 'C';
  assert(++xN <= XMMAX);

  printf("\nC : %2d %4zu %p\n", xN-1, memb*n, p);
  xlive();
  return p;
}
#define calloc xcalloc

void* xrealloc(void* p, size_t z) {
  static void* (*f)(void*, size_t);
  if (!f) f= dlsym(RTLD_NEXT, "realloc");
  assert(f);
  int n= xfind(p);
  printf("\nFR: %2d %4zu %p\n", n, z, p);
  void* v= f(p, z);
  int nn= 0;
  if (v!=p) {
    if (n) {
      if (xS[n]=='F') {
	printf("\n%%ERROR: xrealloc free twice\n");
	exit(1);
      }
	
      xS[n]= 'F';
    }
    xP[xN]= v; xS[xN]= 'R'; nn=xN;
    assert(++xN <= XMMAX);
  }
  printf("MR: %2d %4zu %p\n", xN, z, v);
  xlive();
  return v;
}
#define realloc xrealloc

void xfree(void* p) {
  int n= xfind(p);
  printf("\nF : %2d %4ld %p\n", n, -1L, p);
  static void* (*f)(void*);
  if (!f) f= dlsym(RTLD_NEXT, "free");
  assert(f);

  if (p) {
    if (xS[n]=='F') {
      printf("\n%%ERROR: xfree twice\n");
      exit(1);
    }
    xS[n]= 'F';
  }

  f(p);
  xlive();
}
#define free xfree

// NAN boxing and unboxing of data
#include <stdio.h>
#include <math.h>
#include <assert.h>

// only for *this* endian...
typedef union data {
  uint64_t u;
  double d;
  struct str { char ch[6]; char t: 3; } str;
} data;

// 48 bits of data
#define MTYP 0x0007000000000000L
#define MNAN 0x7ff8000000000000L
#define MDAT 0x0000ffffffffffffL
#define MNEG 0x8000000000000000L

// TYPES:
//   0: NaN         (could be used)
//   1: short atom  char[6]
// not ipmlemented (yet):
//   2: long atom   offset atom heap
//   3: short str   char[6] (==1)
//   4: long str    offset str heap
//   5: void*       offset data heap
//   6: closure     ix closure array
//   7: function    ix func array
//
// Negative: mean need to be GC:ed

#define TYP(x) ((int)((x).u>>48) & 7)
#define NEG(x) (x.u & MNEG)
#define BOX(n,t,dat) ((data){.u=(MNAN | ((n)?MNEG:0) | (((t)&7L)<<48) | ((dat)&MDAT))})
#define DAT(x) (x.u & MDAT)

// Encodes max 6 chars as a nan data
// More than 6 chars are truncated
data char6nan(char* s) {
  data r= BOX(0, 1, 0);
  strncpy((char*)&r.str.ch, s, sizeof(r.str.ch));
  return r;
}

// Return a static string copy of
// string encoded as NaN.
//   Returns NULL if not string (T!=0)
char* nanchar6(data d) {
  static char s[7]= {0};
  if (TYP(d)!=1) return NULL;
  return memcpy((char*)s, d.str.ch, sizeof(d.str.ch));
}

// Make a data atom
// If S is <=6 chars => achar6nan
// Otherwise, string heap is searched
// and offset returned to string.
data atom(char* s, char* atomheap, size_t size) {
  char* p= atomheap; int l= strlen(s);
  if (l<=6) return char6nan(s);
  while(*p) {
    if (0==strcmp(s, p)) return BOX(1, 2, p-atomheap);
    p+= strlen(p)+1;
  }
  assert(p+l+2-atomheap < size);
  strcpy(p, s); p[l+1]= 0; // end
  return BOX(0, 2, p-atomheap);
}

char* nanstr(data d, char* heap) {
  switch(TYP(d)) {
  case 1: return nanchar6(d);
  case 2: printf("FOOBAR\n"); return DAT(d)+heap;
  default: return NULL;
  }
}

// ENDWCOUNT

#ifdef nantypesTEST
void P(char* desc, data d, char* heap) {
  //printf("TYPE=%d\n", d.str.t);
  printf("T=%d %5f 0x%lx '%s' %s\n", TYP(d), d.d, d.u, nanstr(d, heap), desc);
}

int main() {
  data n= {.u=MNAN};
  data f= BOX(1, 7, MDAT);
  data c= char6nan("gurkafoobar");
  data s= BOX(1, 7, 4711);
  P("n  ", n, NULL);
  P("f  ", f, NULL);
  P("c  ", c, NULL);
  P("s  ", s, NULL);
  P("nil", char6nan("nil"), NULL);
  P("null", char6nan("null"), NULL);
  P("undef", char6nan("undef"), NULL);

  const int HSIZE= 16*1024;
  char heap[HSIZE]= {0};
  data shortatom= atom("foobar", heap, HSIZE);
  P("shortatom", shortatom, heap);
  data longatom= atom("foobarfiefum", heap, HSIZE);
  P("longatom", longatom, heap);
}
#endif

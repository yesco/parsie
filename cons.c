// cons.c
//
// Javascript prototype style objects
//
// using cons?

// Just a "fancy" asssoc list

// Property names are of type D so
// can be typed objects, in JS keys are
// converted to strings. We don't.

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef consTEST // DEBUG
  int debug= 0; // DEBUG
  #define DEBUG(D) if (debug) do{D;}while(0);

  #include "mem.c"
  #include "nantypes.c"
#endif // DEBUG

// TODO: use Memory/Stack
D cons(D car, D cdr) {
  D r= BOX(TCONS, C-K);
  *C++= car; *C= cdr;
  return r;
}

D car(D c) { D* p= PTR(TCONS, c); return p?p[0]:error; }
D cdr(D c) { D* p= PTR(TCONS, c); return p?p[1]:error; }
  
// Set in direct obj
// if val is undef, name is removed
D set(D d, D name, D val) {
  D* c= PTR(TCONS,d),*last=0,*p=c;
  if(!c)return undef; // HMMM
  while(p){
    if(deq(*p,name)) {
      p[0]= val==undef?undef:name;
      p[1]= val;
      return val;
    }
    last=p; p=PTR(TCONS,cdr(*p));
  }
  // insert new one?
  // TODO:
}

// Search obj first, then proto...
D get(D d, D name) {
  D *c= PTR(TOBJ, d), *p=o;
  while(p) {
    if(deq(car(p),name)) return p->np[i].val;
    p= PTR(TOBJ,p->next);
  }
  return o?get(o->proto, name):undef;
}

// ENDWCOUNT

void _probj(int indent, D d) {
  Obj* o= PTR(TOBJ,d); if(!o)return;
  printf("%*s---OBJ %ld %p\n", indent-2, "", DAT(d), o);
  printf("%*s---proto ", indent, ""); dprint(o->proto); printf("\n");
  //_probj(indent+2, o->proto);
  _probj(indent+2, 0);
  // TODO: arr - could have array
  //   array usage is very slow
  //   keys would also be unordered
  // TODO: reserved
  //   potentially variable NPN ?
  printf("%*s---props\n", indent, "");
  for(int i=0; i<NPN; i++) {
    printf("%*s[%d] ", indent, "", i);
    int n= dprint(o->np[i].name);
    printf("%*s: ", 16-n, "");
    dprint(o->np[i].val);
    printf("\n");
  }
  printf("%*snext>>>\n", indent, "");
  _probj(indent, o->next);
}
       
D probj(D d) {
  if (TYP(d)!=TOBJ) { printf("---NOT TOBJ: "); dprint(d); putchar('\n'); return d; }
  _probj(2, d);
  printf("---END OBJ\n");
  return d;
}

// Cons
//

#include <stdio.h> // DEBUG
#include <stdlib.h> // DEBUG

typedef double D; // DEBUG

#define CSZ 16*1024

// all cells are identified by CO+i
#define CO 1000000
D *C= 0, nil=CO; int cn= 2;

void initarrcons() { C= calloc(CSZ,sizeof(D)); }

D cons(D a, D d) { int i= C[0]; if (!i) { i=cn; cn+=2;} else { C[0]= C[i]; }
  C[i++]= a; C[i++]= d; return i-2+CO; }
       
int consp(D c) { return c>CO; }
D car(D c) { return C[(int)c+0-CO]; }
D cdr(D c) { return C[(int)c+1-CO]; }

void cfree(D c) {int i=c-CO; D next=C[0]; C[0]=c-CO; C[i+0]=next; C[i+1]=nil; }

void p(D c); // FORWARD

#define P printf
void pl(D c) { P("("); p(car(c)); while(consp((c=cdr(c)))) {P(" ");p(car(c));}
  if (c!=nil) { P(" . "); p(c); } P(")"); }

void p(D c) {if(consp(c)) pl(c); else if(c==nil) P("nil"); else P("%.8g", c); }

// ENDWCOUNT

int main () {
  initarrcons();
  
  D a= cons(1,2);
  p(a); printf("\n");

  D b= cons(a, a);
  p(b); printf("\n");

  D l= cons(1, cons(2, cons(3, nil)));
  p(l); printf("\n");

  
}

#ifdef consTEST
int main(void) {
  initmem(16*1024); inittypes(); 
  
  /// TODO: this is wrong!!!
  if (!deq(nil,nil)) printf("nil doesn't eq\n");
  if (!deq(undef,undef)) printf("undef doesn't eq\n");
  if (deq(undef,nil)) printf("undef and nil eq\n");

  printf("\n=== PRINTF %%g\n");
  printf("%.7g\n", nil);
  printf("%.7g\n", undef);
  printf("%.7g\n", get(0, 42));
  
  printf("\n=== PRINT NULL\n");
  dprint(nil); printf("\n");
  dprint(undef); printf("\n");
  dprint(get(0, 42)); printf("\n");
  probj(0);

  printf("\n=== OOOOOOOO\n");
  D o= obj();
  probj(o);
  printf("get before set: "); dprint(get(o, 42)); printf("\n");
  printf("set           : "); dprint(set(o, 42, 99)); printf("\n");
  printf("get after set : "); dprint(get(o, 42)); printf("\n");
  probj(o);

  printf("=== SETTING 10\n");
  for(int i=0; i<10; i++)
    set(o, i, 100-i);

  printf("=== SETTING nil/undef\n");
  set(o, nil, 666);
  set(o, undef, -666);
  probj(o);

  printf("\n=== INHERITED\n");
  D s= obj(); Obj* sp= PTR(TOBJ,s); sp->proto= o;
  probj(s);
  
  printf("\n=== SET INHERITED\n");
  printf("get 42 before    : "); dprint(get(s, 42)); printf("\n");
  set(s, 42, 420);
  printf("get 42 after set : "); dprint(get(s, 42)); printf("\n");
  printf("get 42 from orig : "); dprint(get(o, 42)); printf("\n");
  set(s, 77, -777);
  printf("get 77 after set : "); dprint(get(s, 77)); printf("\n");
  probj(s);

  printf("\n=== SET MANY\n");
  for(int i=0; i<100; i++)
    set(o, i, 100-i);
  probj(o);

  printf("\n=== ATOMS?\n");
  D x= obj();
  printf("get foo before     : "); dprint(get(x, atom("foo"))); printf("\n");
  set(x, atom("foo"), 123);
  printf("get foo after set  : "); dprint(get(x, atom("foo"))); printf("\n");
  set(x, atom("bar"), 333);
  set(x, atom("foo"), 321);
  printf("get foo after 2ndset: "); dprint(get(x, atom("foo"))); printf("\n");
  probj(x);
  printf("\n=== UNSET\n");
  set(x, atom("foo"), undef);
  printf("get undef foo: "); dprint(get(x, atom("foo"))); printf("\n");
  probj(x);
  
  probj(x);

  dprint(undef); printf("\n");
  dprint(nil); printf("\n");
  dprint(atom("foo")); printf("\n");
  dprint(atom("bar")); printf("\n");
}
#endif


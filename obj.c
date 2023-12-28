// Obj.c
//
// Javascript prototype style objects

// Implemented as chunk of NPN (6)
// named properties, basically an
// "assoc"-chunk. Overflows are found
// by following next pointer.
//
// The proto property allows for
// inheritence:
//
// Get: finds named propoerty in object
//   or in proto object chain.
// Set: sets/replaces the property
//   binding in the local object.
//
// Property names are of type D so
// can be typed objects, in JS keys are
// converted to strings. We don't.

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int debug= 0; // DEBUG
#define DEBUG(D) if (debug) do{D;}while(0);

#include "mem.c"
#include "nantypes.c"

#define NPN 6 // => 6*2+4=>16*8B=128B

// TODO: change pointers to D ?
typedef struct Obj { struct Obj *proto, *next; D arr, reserved;
  struct np { D name, val; } np[NPN]; } Obj;

// TODO: use Memory/Stack
Obj* obj() { Obj* o= calloc(sizeof(Obj), 1);
  for(int i=0; i<NPN; i++)  o->np[i]=(struct np){undef, undef};  return o; }

// Set in direct obj
D set(Obj* o, D name, D val) { if (!o) return undef; Obj *last= 0, *p= o;
  while(p) { for(int i=0; i<NPN; i++) {
      D n= p->np[i].name;
      if (n==name || n==undef) {p->np[i]=(struct np){name,val}; return val; }
    }
    last= p; p= p->next;
  }
  // need one more chunk
  return set((last->next=obj()), name, val);
}

// Search obj first, then proto...
D get(Obj* o, D name) { if (!o) return undef; Obj* p= o;
  // search obj specific props
  while(p) { for(int i=0; i<NPN; i++)
      if (p->np[i].name==name) return p->np[i].val;  p= p->next; }
  // Obj all the way up
  // TODO: rename Obj to Turtle? :-D
  return get(o->proto, name);
}

// ENDWCOUNT

void _probj(int indent, Obj* o) {
  if (!o) return;
  printf("%*s---proto\n", indent, "");
  _probj(indent+2, o->proto);
  // TODO: arr
  // TODO: reserved
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
       
Obj* probj(Obj* o) {
  if (!o) return 0;
  printf("\n---OBJ\n");
  _probj(2, o);
  printf("---END\n");
  return o;
}

int main(void) {
  inittypes(); initmem(1024);
  
  /// TODO: this is wrong!!!
  if (nil!=nil) printf("nil doesn't eq\n");
  if (undef!=undef) printf("undef doesn't eq\n");
  if (undef==nil) printf("undef and nil eq\n");

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
  Obj* o= obj();
  printf("get before set: "); dprint(get(o, 42)); printf("\n");
  printf("set           : "); dprint(set(o, 42, 99)); printf("\n");
  printf("get after set : "); dprint(get(o, 42)); printf("\n");
  probj(o);

  for(int i=0; i<10; i++)
    set(o, i, 100-i);

  set(o, nil, 666);
  set(o, undef, -666);
  probj(o);

  printf("\n=== INHERITED\n");
  Obj* s= obj(); s->proto= o;
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
  Obj* x= obj();
  printf("get foo before     : "); dprint(get(s, atom("foo"))); printf("\n");
  set(x, atom("foo"), 123);
  printf("get foo after set  : "); dprint(get(s, atom("foo"))); printf("\n");
  set(x, atom("bar"), 123);
  set(x, atom("foo"), 321);
  printf("get foo after 2ndset: "); dprint(get(s, atom("foo"))); printf("\n");
}

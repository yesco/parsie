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

#ifdef objTEST // DEBUG
  int debug= 0; // DEBUG
  #define DEBUG(D) if (debug) do{D;}while(0);

  #include "mem.c"
  #include "nantypes.c"
#endif // DEBUG

#define NPN 6 // => 6*2+4=>16*8B=128B

// TODO: change pointers to D ?
typedef struct Obj{D proto,next,arr,n;struct np{D name,val;}np[NPN];}Obj;

// TODO: use Memory/Stack
D obj() { align(); Obj* o= (Obj*)H; H+= sizeof(Obj); memset(o,0,sizeof(Obj));
  // TODO: use m() and K() etc...
  for(int i=0; i<NPN; i++)  o->np[i]=(struct np){undef, undef};
  UL x= ((char*)o)-M; return u2d(BOX(TOBJ, x)); }

// Set in direct obj
// if val is undef, name is removed
D set(D d, D name, D val) {Obj*o=PTR(TOBJ,d),*last=0,*p=o;if(!o)return undef;
  if (deq(name,proto)) return p->proto=val; if (!ISNAN(name) && name >= o->n) o->n=name+1;
  while(1){for(int i=0;i<NPN;i++){D n=p->np[i].name; if(deq(n,name)||deq(n,
      undef)){p->np[i]=(struct np){val==undef?undef:name,val};return val;
    }} last=p;p=PTR(TOBJ,p->next);if (!p){p=PTR(TOBJ,last->next=obj());}}}

// Search obj first, then proto...
D get(D d, D name) {Obj *o=PTR(TOBJ,d),*p=o; if(deq(name,proto))return p->proto;
  while(p) {for(int i=0;i<NPN;i++) if(deq(p->np[i].name,name)) return p->np[i].val;
    p= PTR(TOBJ,p->next); }  return o?get(o->proto, name):undef;
}

D probj(D); // FORWARD

// print numeric properties ( array ? )
// TODO: not fully correct
// TODO: technically node has holes
int pobj(D d){Obj*o=PTR(TOBJ,d),*p=o;if(!o)return 0;int i=0,n=P(o->n?"[":"{");
  if (o->n) { D last=nil; int u=0; for(int i=0; i<o->n; i++) {D v= get(d, i);
    if(deq(v,undef)&&++u>0)continue; if(u>1)n+=P("<%d empty items>, %d:",u,i);
    u=0;n+=dprint(v)+P(", ");last=v;}}  while(p){i=0;for(D*x;x=&(p->np[i].name),
      i<NPN;i++)if(!deq(*x,undef)&&ISNAN(*x)){n+=dprint(*x)+P(":")+dprint(x[1])
      +P(" ");} p=PTR(TOBJ,p->next);}n+=P(o->n?"]":"}");return n;}

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
  printf("%*s---n=%g\n", indent, "", o->n);
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
  if (TYP(d)!=TOBJ) { printf("---NOT TOBJ: "); dprint(d); pc('\n'); return d; }
  _probj(2, d);
  printf("---END OBJ\n");
  return d;
}

#ifdef objTEST
int main(void) {
  inittypes(); initmem(1024);
  
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


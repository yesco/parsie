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

#ifndef DEBUG // DEBUG
  int debug= 0; // DEBUG
  #define DEBUG(D) if (debug) do{D;}while(0);

  #include "mem.c"
  #include "nantypes.c"
#endif // DEBUG

#define NPN 6 // => 6*2+4=>16*8B=128B

// TODO: change pointers to D ?
typedef struct Obj{D proto,next,arr,n;struct np{D name,val;}np[NPN];}Obj;

D obj() { Obj* o= (Obj*)C; C+= sizeof(Obj)/SL; memset(o,0,sizeof(Obj));
  for(int i=0; i<sizeof(Obj)/SL; i++) GCA(((D*)o)+i);
  // TODO: use m() and K() etc...
  for(int i=0; i<NPN; i++)  o->np[i]=(struct np){undef, undef};
  UL x= ((D*)o)-K; RET u2d(BOX(TOBJ, x)); }

Obj* po(D o) { return TYP(o)==TOBJ?(Obj*)(K+DAT(o)):0; }

D* setmark(D*,D*); // FORWARD

// Set in direct obj
// if val is undef, name is removed
//
// TODO: delete? how does it work?
//   node: removes entry maybe set to
//   *hole* or *deleted* ???
// TODO: problem is reused and duplicate
D set(D d, D name, D val) {Obj*o=po(d),*last=0,*p=o;if(!o)RET undef;
  if(deq(name,proto))RET p->proto=val;if(!ISNAN(name)&&name>=o->n)o->n=name+1;
  while(1){for(int i=0;i<NPN;i++){D n=p->np[i].name; if(deq(n,name)||deq(n,
      undef)){
//csetmark(d,p->np[i].val,val);
setmark(&(p->np[i].val),&val);
p->np[i]=(struct np){name,val};RET val;
    }} last=p;p=po(p->next);if (!p){p=po(last->next=obj());}}}

// TODO: .length set and get
//   only set on Array not Object

// Search obj first, then proto...
D get(D d, D name) {Obj *o=po(d),*p=o; if(deq(name,proto))RET p->proto;
  while(p) {for(int i=0;i<NPN;i++) if(deq(p->np[i].name,name)) RET p->np[i].val;
    p= po(p->next); }  RET o?get(o->proto, name):undef;
}

D probj(D); // FORWARD

// print numeric properties ( array ? )
// TODO: not fully correct
// TODO: technically node has holes
int pobj(D d){Obj*o=po(d),*p=o;if(!o)RET 0;int i=0,n=P(o->n?"[":"{");
  int s=0; if(o->n){ D last=nil;int u=0;for(int i=0;i<o->n;i++){D v=get(d,i);
  if(deq(v,undef)&&++u>0&&i+1!=o->n)continue; if(u>1){n+=(s?P(" "):0)
   +P("<%d empty items>",u);s=1;if(i+1==o->n)break;P(" %d:",i);}
   u=0;n+=(s?P(" "):0)+dprint(v);s=1;last=v;}}
   while(p){i=0;for(D*x;x=&(p->np[i].name),i<NPN;i++)if(!deq(*x,undef)&&ISNAN(*x
     )){n+=(s?P(" "):0)+dprint(*x)+P(":")+dprint(x[1]);s=1;}
     p=po(p->next);}n+=P(o->n?"]":"}");RET n;}

// ENDWCOUNT

void _probj(int indent, D d) {
  Obj* o= po(d); if(!o)RET;
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
  if (TYP(d)!=TOBJ) { printf("---NOT TOBJ: "); dprint(d); pc('\n'); RET d; }
  _probj(2, d);
  printf("---END OBJ\n");
  RET d;
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


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





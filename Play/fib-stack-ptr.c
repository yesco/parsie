// "sackie" -O3 35 => 35.56s!!!
// fib.c is 1.59s..

// threaded is slowest implementation

// 35 => 3.12s
// 
// ual  35=> 4.09s
//   (full bytecode impl)

// ual-vars (interpret+registers)
//      35=> 2.52s  40=> 27.81s
// fib  35=> 2.96s  40=> 35.56s
// emcs 35=> 3.51s  40=> 35.63s

// fib2 35=> 0.52s  40=>  5.58s
//   (COMPILED calls/inline?)

// stck 35=> 0.66s  40=>  5.72s
//   (not complet, no interpr)
// vars 35=> 0.49s  40=>  3.95s
//   (not complet, no interpr)

// as "complete" interpreter
// ual-vars REGISTER VM wins: 2.95s
// faster than UAL
#include <stdio.h>
#include <assert.h>

double K[1024]={666,0},*S=K;

#define R return

typedef void(*F)();

F prog[1024], *p;

void interpret();

void zero  () { S[1]=0; S++; }
void one   () { S[1]=1; S++; }
void two   () { S[1]=2; S++; }

void dup   () { S[1]=*S; S++; }
void swap  () { S[1]=*S; *S=S[-1]; S[-1]= S[1]; }
void plus  () { S--; *S=*S+S[1]; }
void minus () { S--; *S=*S-S[1]; }
void eq    () { S--; *S=*S==S[1]; }

void zskip () { p+= !*S; S--; }

// not totally fair, no return stack?
void fib   () { F* s=p; p= prog+1; interpret(); p=s; }

const F ret= 0;

#define X(n) {n, #n}

struct {
  F f; char* name;
} funame[] = {
  X(interpret),
  X(zero),X(one),X(two),
  X(dup),X(swap),
  X(plus),X(minus),
  X(eq),
  X(zskip),X(ret),
  X(fib),
};

char* fname(F f) {
  for(int i=0; i<sizeof(funame)/sizeof(funame[0]); i++)
    if (funame[i].f==f) return funame[i].name;
  return 0;
}

void pr() {
  printf("\t -- ");
  for(double* s= K; s<=S; s++)
    printf(" %.8g", *s);
  printf("\n");
}

F prog[1024]={
  0,

  // -- fib
  dup, zero, eq,
  zskip, ret,

  dup, one, eq,
  zskip, ret,

  dup, one, minus, fib,
  swap, two, minus, fib, 

  plus, ret,
};

void fib2() {
  dup(); zero(); eq();
  if (*S--) return;
  
  dup(); one(); eq();
  if (*S--) return;
  
  dup(); one(); minus(); fib2();
  swap(); two(); minus(); fib2();
  plus(); 
  return;
}

void interpret() {
  while(*p) {
    //pr(); printf("\t\t\t\t%2ld %s:\n", p-prog, fname(*p));
    (*p++)();
  }
}

int main(int argc, char** argv) {
  double n= atoi(argv[1]);
  *++S= n;
  if (0) {
    fib2(n);
  } else {
    p=prog+1; interpret();
  }
  double r= *S--;
  printf("fib %f => %f\n", n, r);
}

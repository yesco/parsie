// "sackie" -O3 35 => 0.66s
#include <stdio.h>
#include <assert.h>


double K[1024]={666,0},*S=K;

void pr() {
  printf("\t -- ");
  for(double* s= K; s<=S; s++)
    printf(" %.8g", *s);
  printf("\n");
}

void fib() {
  double n= *S;
  // if (n<=0) return 0;
  S[1]= *S; S++; // dup
  //assert(*S==n);
  *++S= 0;
  S--;
  *S= *S==S[1];
  if (*S--) { return; }

  // else if (n==1) return 1;
  S[1]= *S; S++; // dup
  //assert(*S==n);
  *++S= 1;
  S--;
  *S= *S==S[1];
  if (*S--) { return; }

  // else return fib(n-1)+fib(n-2);
  // - fib(n-1)
  S[1]= *S; S++; // dup
  //assert(*S==n);
  *++S= 1;
  S--; *S= *S-S[1];
  fib();

  // swap
  double t=*S; *S= S[-1]; S[-1]= t;

  // - fib(n-2)
  //assert(*S==n);
  *++S= 2;
  S--; *S= *S-S[1];
  fib();

  // fib(n-1)+fib(n-2)
  S--; *S= *S+S[1];
  return;
}

int main(int argc, char** argv) {
  double n= atoi(argv[1]);
  *++S= n;
  fib();
  double r= *S--;
  printf("fib %f => %f\n", n, r);
}

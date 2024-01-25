// "vars" -O3 35=>  0.49s 40=> 3.95s
#include <stdio.h>
#include <assert.h>


double K[1024]={666,0},*S=K;

void pr(double* S) {
  printf("\t -- ");
  for(double* s= K; s<=S; s++)
    printf(" %.8g", *s);
  printf("\n");
}

void fib(double* S) {
  // if (n<=0) return 0;
  S[1]= 0;
  S[2]= S[0]==S[1];
  if (S[2]) { S[1]=S[0]; return; }

  // else if (n==1) return 1;
  S[3]= 1;
  S[4]= S[0]==S[3];
  if (S[4]) { S[1]=S[0]; return; }

  // else return fib(n-1)+fib(n-2);
  // - fib(n-1)
  //assert(*S==n);
  S[5]= 1;
  S[6]= S[0]-S[5];
  fib(S+6);
  // result in S[7]

  // - fib(n-2)
  S[8]= 2;
  S[9]= S[0]-S[8];
  fib(S+9);
  // result in S[10]

  // fib(n-1)+fib(n-2)
  S[11]= S[7]+S[10];
  S[1]= S[11];
  return;
}

int main(int argc, char** argv) {
  double n= atoi(argv[1]);
  K[1]= n;
  fib(K+1);
  double r= K[2];
  printf("fib %f => %f\n", n, r);
}

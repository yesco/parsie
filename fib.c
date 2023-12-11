#include <stdio.h>

double fib(double n) {
  if (n<=0) return 0;
  else if (n==1) return 1;
  else return fib(n-1)+fib(n-2);
}

int main(int argc, char** argv) {
  double n= atoi(argv[1]);
  double r= fib(n);
  printf("fib %f => %f\n", n, r);
}

// Fibonacci program equivalent to fib.pas
// 44x faster than fib.pas.alf ... lol

#include <stdio.h>

int F(int n) {
  if (n<=0) return 0;
  else if (n==1) return 1;
  else return F(n-1)+F(n-2);
}

int main(void) {
  int x= 42;
  int a= 35;
  int r;

  printf("PROGRAM Fibonacci running:\n");

  printf("FIB: ");
  printf("%d ", F(0));
  printf("%d ", F(1));
  printf("%d ", F(2));
  printf("%d ", F(3));
  printf("%d ", F(4));
  printf("%d ", F(5));
  printf("%d ", F(6));
  printf("%d ", F(7));
  printf("%d ", F(8));
  printf("%d ", F(9));
  printf("%d\n", F(10));

  r= F(a);
  printf("Fib( %d ) = %d\n", a, r);

  return 0;
}


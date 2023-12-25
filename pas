# ./pas fib 35 is about 56% slower than ./fib 35 dup fib...

clang pars-nice.c && ./a.out -d $* <pars-nice-pascal-fib.tst | tail -1 > .alf && clang -O3 alf.c && time -p ./a.out < .alf


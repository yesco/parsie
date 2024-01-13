echo "LOC: `./wcode al.c`"
clang -DalTEST -O3 al.c && time -p ./a.out $*




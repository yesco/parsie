echo "Al/F.LOC: `./wcode al.c`"
echo "AlF.LOC: `./wcode alf.c`"
clang -DalTEST -O3 al.c && time -p ./a.out $*




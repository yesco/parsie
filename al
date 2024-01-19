l=$(./wcode al.c)
alfbody=$(./wcode alf-body.c)

alalf=$(echo "$l+$alfbody"|bc -l)

printf "Al/F.LOC: $alalf\n"
printf "AlF .LOC: $l\n"
clang -DalTEST -O3 al.c && time -p ./a.out $*




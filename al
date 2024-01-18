l={./wcode al.c}
abody={./wcode alf-body.c}

alalf=$(echo "$l+$alfbody"|bc -l)

echo "Al/F.LOC:\t$alalf`"
echo "AlF.LOC:\t$l"
clang -DalTEST -O3 al.c && time -p ./a.out $*




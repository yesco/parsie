A=a :z=1 [11]
B=b :z=2 [22]
C=c :z=3 [33]
D=d :z=3 [33]
# how to inherit attributes so not get overwritten?
Y= A [$1]
 | B [$1]
 | C [$1]
 | D [$1]
X=YYYY [$1 $2 $3 $$1z $$2z $$3z]
?X
abca
cbaa
baca



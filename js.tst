?J
function F(n) { G(3); } function G(n) { return 42; }
function F(n) {  if (n==0) return 0;  else if (n==1) return 1;  else return F(n-1)+F(n-2); }
?S
#console.log(F(42));

?L
1
1,2
1,2,3
# BUG: one undef
,
# BUG: gives a c ?
,,
# ok
,,3
1,,,,,,,,,,11

?M
[]
[1]
[1,2]
[1,2,3]
[1,,3]
[,2,3]
[1,2,,]
[,2,,4,]


D=%d
L=a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_
A=D,A|D

?D
0
3
44

?A
1
1,2
1,2,3

O=a7|a
?O
a7
a

O=a?2b
?O
x
a
a2b
a2
a22
a22b
ab

R=a*D
?R
a
a1
a12
a123
a1234b

# BUG: test
A=%d|%a
?A
1
b

# ???
X=b
B=a?Xc
?B
a
abc
ac
abbc
abbbc

# +++
X=b
B=a+Xc
?B
a
abc
ac
abbc
abbbc

# ***
X=b
B=a*Xc
?B
a
abc
ac
abbc
abbbc

# capture
B=%n
C=bar
X=foo B C[A $1 B $2 C]
?X
foo bar
foo fie bar
foo fie 33 bar

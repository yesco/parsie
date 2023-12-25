# ok
X=%d
?X
1
3

X=%a
a
b

X=%w
a
b
_
6

X=%i
3
123

X=%n
a
abc
_ab7
_

# ----------- fail
X=%d
?X
q
33

X=%a
3
_
-

X=%w
-
;

X=%i
one
+

X=%n
9qb
3
-

X=%e
?X
*
x
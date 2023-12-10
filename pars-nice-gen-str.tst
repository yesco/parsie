K=%s
?K
"foo"

K=%s[$1]
?K
"foo"

K=%s
E=K[$1]
?E
"foo"

# not working!
K=%s[$1]
E=K[$1]
"foo"

# not working!
K=%s[$1]
E=K[$'1]
?E
"foo"

# not working!
K=%s[$1]
E=K[$1]
?E
"foo"
"bar"

# not working!
K=%"[$1]
E=K[ab$1ba]
?E
"foo"
"bar"

# not working!
A=foo[111]
?A
foo

# not working!
B=A[$1]
?B
foo

C=B[$1]
?C
foo

D=C[$1]
?D
foo

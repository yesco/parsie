Q=%"
?Q
"foo"

S=%"[$1]
?S
"foo"

B=b[42]
A=a[69]
S=B%"A[$1 x $2 x $3]
?S
b"foo"a
b'bar'a
b<fie>a
   b"foo"a


U=B%'A[$1 y $2 y $3]
?U
b"foo"a
b'bar'a
b<fie>a
   b"foo"a

S=B%sA[$1 z $2 z $3]
?S
b"foo"a
b'bar'a
b<fie>a
   b"foo"a
b(foo)a
b{bar}a
b[fie]a
b(foo(bar)a
b{foo{bar}a
b[foo[bar]a

# quoted
S=%s[$'1]
?S
"foo bar"
"foo\"bar"
"foo'bar"
<fi'<\>sh>

S=%s[$<1]
"foo bar"
"foo\"bar"
"foo'bar"
<fi'<\>sh>


# PASCAL bnf -> ALF
#
# (free: Z) LOL

# TODO: preprocess comments?

N=%n
D=%i
K=%s

# math priority ( ) >>> * / >>> + -

# Logical negation: not
# Multiplicative: * / div mod and
# Additive: + - or
# Relational: = <> < > <= >= in

U= D | F [$1] | N ::$1 [$:1 @] | \( E \) [$1]
T= U \* T [$1 $2 *] | U \/ T [$1 $2 /] | U [$1]
E= K [\$$1\\] | T \+ E [$1 $2 +] | T \- E [$1 $2 -] | T [$1]

Z= = | <> [z] | <= [>z] | < | >= [<z] | >
# TODO: and >>> or
# TODO: E Z E may not work
C= E Z E [$1 $3 $2] | not C [$1~] | C and C [$1 $2 &]


Q= integer :t=i8 | char :t=c1 | string :t=s
# TODO: repeated w ,
V= var N\:Q=E; ::$1 V [_$1 $3`$1! $4 ] | var N\:Q; V [_$1 $3 ] |
L= N\:=E :=$1 [$2 $:1 !]
I= if C then S else S [$1?{$2}{$3}] | if C then S [$1?{$2} ]
R= repeat G until C
W= while E do S

X= E,X [$1:print $1 $2] | E [$1:print $1] |
M= write\(K\) [$1] | write\(E\) [$1.] | writeln\(K\) [$1$n] | writeln\(E\) [$1.\$n]|

A= E,A [$1 $2] | E [$1] |
H= N ::$1 ,H | N ::$1 |

# TODO: long function call
#F= N\(A\) [($2) `$1@x]
F= N\(A\) [ $2 F ]

S= B [$1] | \Exit E; [$1^] | \Exit [$1^]; | \Result\:\=E; [$1] | result:=E; [$1] | M; [$1] | L; [$1] | I [$1] | R [$1] | W[$1] | F; [$1] 

G= S G[$1 $2] | S [$1] | ;
# TODO: vars mixed w statements?
# TODO: how are vars allocated in block?(stack?)

B= begin V G end [$1 $2]
# not working with *V

P= program N; B.%e ["$1:"$n $2]

?V
var x: integer;
var a: integer = 42;
var r: integer = "foobar";

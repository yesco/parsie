?"---Simple Test for each Lunction"

?"--  "
 

?"-- !"
64@P 42 64! 64@P
'foo @P 42 'foo ! 'foo @P

?'-- "'
"foo" P
"bar" P

?"-- #"
'nil	# P
3	# P
"foo"	# P
'foo	# P
'()	# P
'(a b)	# P
'[1 2]	# P
'{}	# P

?"-- $"
'nil	$ P
3	$ P
"foo"	$ P
'foo	$ P
'()	$ P
'(a b)	$ P
'[1 2]	$ P
'{}	$ P

?"-- %"
4 2	% P
5 2	% P
100 2	% P
101 2	% P

?"-- &"
0 7 	& P
3 7	& P
0 1- 7	& P

?"-- '"
'nil	U P
'3	# P
'"foo"	$ P
''foo	'? P
'()	U P
'(a b)	K P
'[1 2]	O P
'{}	O P

'nil	P
'3	P
'"foo"	P
''foo	P
'()	P
'(a b)	P
'[1 2]	P
'{}	P

'[a b c[d e f]]	P
'{a:11 b:22 c:[33 d:44]}	P
'[[][[]]]	P
'{[]{}[{[{}]}]}	P

'[,,,,,,,,,42]	P

?"TODO:BUG: setting to undef
'[,,,,,,,,,]	P

?"-- ("


?"-- )"


?"-- *"
3 4 * P

?"-- +"
3 4 + P

?"-- ,"
'[] 11 , 22 , 33 , P

?"-- -"
3 4 - P

?"-- ."
'[00 11 22 33] 0 . P
'[00 11 22 33] 2 . P
'[00 11 22 33] 7 . P

'{a: 11 b: 22 c: 33} 'a . P
'{a: 11 b: 22 c: 33} 'b . P
'{a: 11 b: 22 c: 33} 'c . P
'{a: 11 b: 22 c: 33} 'nil . P

'{a: 11 b: 22 c: 33} "a" . P
'{a: 11 b: 22 c: 33} "b" . P
'{a: 11 b: 22 c: 33} "c" . P

?"-- /"
3 4 / P

?"-- 0"
0P1P2P3P4P5P6P7P8P9P
111P
1234567890P
3.141592654P

?"-- :"
'{} 'a 11 : 'b 22 : 'c 33 : P

?"-- ;"


?"-- <"
3 < 4 P
4 < 3 P
3 < 3 P

"bar" < "bar" P
"bar" < "foo" P

3 "foo" < P
3 '() < P
3 'nil < P
3 '[] < P
3 '[] < P

?"-- ="
3 4 = P
3 3 = P

'foo 'foo = P
'foo 'bar = P
3 'foo = P

"foo" "foo" = P
"foo" "bar" = P

'nil 'nil = P
'() '() = P
'{} '{} = P

?"-- >"
3 4 > P
4 3 > P
3 3 > P

?"-- ?"
?"foo'bar"T
?'foo"bar'T
?[fooz'bar"fie\\fum]T
?">"?_"<"T

?"-- @"
64	@ P
42 64! 64 @P

'nil	@ P
'undef	@ P
"*error*" '$	@ P

'foo	@P
42 'foo	! 'foo	@P

?"-- A"
'nil		A P
'()		A P
'(a . b)	A P
'[]		A P
'{}		A P

?"-- B"
B

?"-- C"
3 4	C P
'a 'b	C P
'a 'nil	C P
1 2 3 4 5 'nil CCCCC P
1 2 3 4 5 6 CCCCC P
'[] '{}	C P

?"-- D"
'nil		D P
'()		D P
'(a . b)	D P
'[]		D P
'{}		DA P

?"-- E"
"3"	E P
"3 4 +"	E P

?"-- F"
666 42 Fd PP F\

?"-- G"
'a '((a . 11) (b . 22)(c . 33)) G P
'b '((a . 11) (b . 22)(c . 33)) G P
'nil '((a . 11) (b . 22)(c . 33)) G P
22 '((a . 11) (b . 22)(c . 33)) G P

?"-- H"
H

?"-- I"
42 0I{"then "} "rest" PW
42 0I{"then "}"rest" PW
42 0I{"then "}{"else "} "rest" PWW
42 0I{"then "}{"else "}"rest" PWW

42 1I{"then "} "rest" PWW
42 1I{"then "}"rest" PWW
42 1I{"then "}{"else "} "rest" PWWW
42 1I{"then "}{"else "}"rest" PWWW

?"-- J"

?"-- K"
nil	K P
3	K P
"foo"	K P
'foo	K P
'()	K P
'(a b)	K P
'[1 2]	K P
'{}	K P


?"-- L"
	0 L P
11	1 L P
11 22	2 L P

?"-- M"
#
'a '(a 1 b 2 c 3 d 4) M P
'c '(a 1 b 2 c 3 d 4) M P
'g '(a 1 b 2 c 3 d 4) M P
'nil '(a 1 b 2 c 3 d 4) M P
'a '() M P

?"-- N"
'(00 11 22 33 44) 0 N P
'(00 11 22 33 44) 1 N P
'(00 11 22 33 44) 4 N P
'(00 11 22 33 44) 5 N P
'(00 11 22 33 44) 0 1- N P
'(00 11 22 33 44) 0 1- 'foo P

'[00 11 22 33 44] 0 N P
'[00 11 22 33 44] 4 N P
'[00 11 22 33 44] 5 N P
'[00 11 22 33 44] 0 1- N P

?"-- O"
'()		O P
'(1 2 3)	O P
'[]		O P
'[1 2 3]	O P
'{a:11,b:22}	O P

?"-- P"
nil	P	nil	P
3	P	3	P
"foo"	P	"foo"	P
'foo	P	'foo	P
'()	P	'()	P
'(a b)	P	'(a b)	P
'[1 2]	P	'[1 2]	P
'{}	P	'{}	P

?"-- Q"


?"-- R"


?"-- S"
'(a . b) Fd	3 SA P  P
'(a . b) Fd	4 SD P  P
'(a . b) FdFdFd	3 SA P  4 SD P  P

?"setting arguments"T
666 42
11 22 33
\xyz xSyxSz xPyWzW xy+z+^
PPF\

?"-- T"
"foo"T"bar"

?"-- U"
nil	U P
3	U P
"foo"	U P
'foo	U P
'()	U P
'(a b)	U P
'[1 2]	U P
'{}	U P

?"-- V"


?"-- W"
'nil	W	'nil	W T
3	W	3	W T
"foo"	W	"foo"	W T
'foo	W	'foo	W T
'()	W	'()	W T
'(a b)	W	'(a b)	W T
'[1 2]	W	'[1 2]	W T
'{}	W	'{}W	W T

?"-- X"
'nil	X	'nil	X T
3	X	3	X T
"foo"	X	"foo"	X T
'foo	X	'foo	X T
'()	X	'()	X T
'(a b)	X	'(a b)	X T
'[1 2]	X	'[1 2]	X T
'{}	X	'{}W	X T

?"-- Y"
?"write 42:"	Y P

?"-- Z"

?"-- ["

?"-- \"
"\ 42^"		E P
666 42 \ a^
P
666 42 \a a^
P
666 42 \x x^
P
666 3 4 \\ ab+^
P
666 3 4 \xy xy+^
P

?"don't use"
666 42 \
P
666 33 \ 42
PP

?"-- ]"

?"-- ^"
?"(see \)"T

?"-- _"


?"-- `"
`nil P
`a P
`b P
`0 P
'1 P
`2 P
'{} P
`[] P

?"-- {"
?"see I"T

?"-- |"
0 7 	| P
3 8	| P
0 1- 7	| P

?"-- }"

?"see {"T

?"-- ~"
0 ~ P
1 ~ P
'nil ~ P
'true ~ P
'a ~ P
"foo" ~ P
'[] - P
'[1] ~ P
'{} ~ P
'{a:33} ~ P

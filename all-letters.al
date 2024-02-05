T?"---Simple Test for each Function"

T?"--  "
 

T?"-- !"
64@P 42 64! 64@P
'foo @P 42 'foo ! 'foo @P

?'-- "'
"foo" P
"bar" P

T?"-- #"
'nil	# P
3	# P
"foo"	# P
'foo	# P
'()	# P
'(a b)	# P
'[1 2]	# P
'{}	# P

T?"-- $"
'nil	$ P
3	$ P
"foo"	$ P
'foo	$ P
'()	$ P
'(a b)	$ P
'[1 2]	$ P
'{}	$ P

T?"-- %"
4 2	% P
5 2	% P
100 2	% P
101 2	% P

T?"-- &"
0 7 	& P
3 7	& P
0 1- 7	& P

T?"-- '"
'nil	U P
'3	# P
'"foo"	$ P
''foo	'? P
'+	'? P
'()	U P
'(a b)	K P
'[1 2]	O P
'{}	O P

'nil	P
'3	P
'"foo"	P
''foo	P
'+	P
'()	P
'(a b)	P
'[1 2]	P
'{}	P

'[a b c[d e f]]	P
'{a:11 b:22 c:[33 d:44]}	P
'[[][[]]]	P
'{[]{}[{[{}]}]}	P

'[,,,,,,,,,42]	P

T?"TODO:BUG: setting to undef"T

'[,,,,,,,,,]	P

T?"-- ("


T?"-- )"


T?"-- *"
3 4 * P

T?"-- +"
3 4 + P

T?"-- ,"
'[] 11 , 22 , 33 , P

T?"-- -"
3 4 - P

T?"-- ."
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

T?"-- /"
3 4 / P

T?"-- 0 -- 9" 
0P1P2P3P4P5P6P7P8P9P
111P
1234567890P
3.141592654P
0x10P
0xffffP
0100P
0b11111111P

T?"-- :"
'{} 'a 11 : 'b 22 : 'c 33 : P

T?"-- ;"


T?"-- <"
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

T?"-- ="
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

T?"-- >"
3 4 > P
4 3 > P
3 3 > P

T?"-- T?"T
T?"foo'bar"T
?'foo"bar'T
?[fooz'bar"fie\\fum]T
T?">"?_"<"T

T?"-- @"
64	@ P
42 64! 64 @P

'nil	@ P
'undef	@ P
"*error*" '$	@ P

'foo	@P
42 'foo	! 'foo	@P

T?"-- A"
'nil		A P
'()		A P
'(a . b)	A P
'[]		A P
'{}		A P

T?"-- B"
'a '(a 1 b 2 c 3 d 4) B P
'c '(a 1 b 2 c 3 d 4) B P
'g '(a 1 b 2 c 3 d 4) B P
'nil '(a 1 b 2 c 3 d 4) B P
'a '() B P

T?"-- C"
3 4	C P
'a 'b	C P
'a 'nil	C P
1 2 3 4 5 'nil CCCCC P
1 2 3 4 5 6 CCCCC P
'[] '{}	C P

T?"-- D"
'nil		D P
'()		D P
'(a . b)	D P
'[]		D P
'{}		DA P

T?"-- E"
"3"	E P
"3 4 +"	E P

T?"-- F"
666 42 Fd PP F\

T?"-- G"
'a '((a . 11) (b . 22)(c . 33)) G P
'b '((a . 11) (b . 22)(c . 33)) G P
'nil '((a . 11) (b . 22)(c . 33)) G P
22 '((a . 11) (b . 22)(c . 33)) G P

?"Tolerance of other stuff..."T

'c '((a . 1) (b . 2) c 33 (c . 3) nil (d . 4)) G P

T?"-- H"

T?"-- I"
42 0I{"then "} "rest" PW
{  TOOD: 42 0I{"then "}"rest" PW  }
42 0I{"then "}{"else "} "rest" PWW
42 0I{"then "}{"else "}"rest" PWW

42 1I{"then "} "rest" PWW
{ TODO: 42 1I{"then "} "rest" PWW }
42 1I{"then "}{"else "} "rest" PWW
42 1I{"then "}{"else "}"rest" PWW

T?"-- J"

T?"-- K"
'nil	K P
3	K P
"foo"	K P
'foo	K P
'()	K P
'(a b)	K P
'[1 2]	K P
'{}	K P


T?"-- L"
	0 L P
11	1 L P
11 22	2 L P

T?"-- M"
"1+" '(10 20 30 40) M P
[\kv k] '[11 22 33 a:7 b:8 c:9] M P
[\kv k] '[__proto__: [44 55 66 a: 1 b:2 c:3] 11 22 33 a:7 d:8 e:9] M P

?"------- ATOM MODIFIERS -----"T

?"=== ALL TRUE ==="T

[10+] '(1 2 3 4 5) M P
[??] '(1 2 3 4 5) 'm M P
[??] '(1 2 3 4 5) 'A M P
[??] '(1 2 3 4 5) 'N M P
[??] '(1 2 3 4 5) 'E M P

?"=== MOST TRUE"T
[10+] '(1 2 0 4 5) M P
[??] '(1 2 0 4 5) 'm M P
[??] '(1 2 0 4 5) 'A M P
[??] '(1 2 0 4 5) 'N M P
[??] '(1 2 0 4 5) 'E M P

?"=== NONE TRUE"T
[10+] '(0 0 0 0 0) M P
[??] '(0 0 0 0 0) 'm M P
[??] '(0 0 0 0 0) 'A M P
[??] '(0 0 0 0 0) 'N M P
[??] '(0 0 0 0 0) 'E M P


?"=== LASTTRUE"T
[10+] '(0 0 0 0 1) M P
[??] '(0 0 0 0 1) 'm M P
[??] '(0 0 0 0 1) 'A M P
[??] '(0 0 0 0 1) 'N M P
[??] '(0 0 0 0 1) 'E M P

?"=== FILTER ===="T
"2%1=" 'odd !
1 _odd P
2 _odd P
3 _odd P

[_odd] '(1 2 3 4 5) 'p M P
'odd '(1 2 3 4 5) 'p M P
'odd @ '(1 2 3 4 5) 'p M P

[??_odd] '(1 2 3 4 5) 'm M P
[??_odd] '(1 2 3 4 5) 'A M P
[??_odd] '(1 2 3 4 5) 'N M P
[??_odd] '(1 2 3 4 5) 'E M P

[??_odd] '(1 3 5) 'm M P
[??_odd] '(1 3 5) 'A M P
[??_odd] '(1 3 5) 'N M P
[??_odd] '(1 3 5) 'E M P

[??_odd] '(2 4) 'm M P
[??_odd] '(2 4) 'A M P
[??_odd] '(2 4) 'N M P
[??_odd] '(2 4) 'E M P

?"=== FOLD w initial val ==="T

'nil [2L] '(1 2 3 4 5) 'ir M P

0 [\ab a??b??+T^] '(1 2 3 4 5) 'ir M P

1 [*] '(1 2 3 4 5) 'ir M P

100 [+] '(1 2 3 4 5) 'ir M P
100 [*] '(1 2 3 4 5) 'ir M P

1 [*] '() 'ir M P
1 [*] '(3) 'ir M P

?"=== FOLDR ==="T

[2L] '(1 2 3 4 5) 'r M P

[\ab a??b??+T^] '(1 2 3 4 5) 'r M P

[*] '(1 2 3 4 5) 'r M P
[+] '(1 2 3 4 5) 'r M P

[*] '() 'r M P
[*] '(3) 'r M P

?"-COPY-"T
'nil 'C '(1 2 3 4 5) 'ir M P

?"=== FOLDL w initial val ==="T

'nil [2L] '(1 2 3 4 5) 'il M P

0 [\ab a??b??+T^] '(1 2 3 4 5) 'il M P

0 "'+ '(1 2 3 4 5) 'il M P"

1 [*] '(1 2 3 4 5) 'il M P

100 [+] '(1 2 3 4 5) 'il M P
100 [*] '(1 2 3 4 5) 'il M P

?"=== FOLDL ==="T

[2L] '(1 2 3 4 5) 'l M P

[\ab a??b??+T^] '(1 2 3 4 5) 'l M P

"'+ '(1 2 3 4 5) 'l M P"

[*] '(1 2 3 4 5) 'l M P

[+] '(1 2 3 4 5) 'l M P
[*] '(1 2 3 4 5) 'l M P

[*] '() 'l M P
[*] '(3) 'l M P
[*] '(3 4) 'l M P

?"-reverse using FOLDL-"T

'nil [FsC] '(1 2 3 4 5) 'il M P
'nil [\ab baC] '(1 2 3 4 5) 'il M P

T?"-- N"
0 '(00 11 22 33 44)	N P
1 '(00 11 22 33 44)	N P
4'(00 11 22 33 44)	N P
5 '(00 11 22 33 44)	N P
0 1- '(00 11 22 33 44)	N P
'foo '(00 11 22 33 44)	N P

0 '[00 11 22 33 44]	N P
1 '[00 11 22 33 44] 	N P
5 '[00 11 22 33 44] 	N P
0 1- '[00 11 22 33 44]	N P

T?"-- O"
'()		O P
'(1 2 3)	O P
'[]		O P
'[1 2 3]	O P
'{a:11,b:22}	O P

T?"-- P"
'nil	P	'nil	P
3	P	3	P
"foo"	P	"foo"	P
'foo	P	'foo	P
'()	P	'()	P
'(a b)	P	'(a b)	P
'[1 2]	P	'[1 2]	P
'{}	P	'{}	P

T?"-- Q"
3 3		Q P
3 1 0 /		Q P
"foo" "foo"	Q P
"foo" 3		Q P
"foo" 'foo	Q P
"foo" '()	Q P
'() '()		Q P
'(a) '()	Q P
'(a) '(a)	Q P
'(a) '(b)	Q P
'(a . b) '(a . b)	Q P
'(a b) '(a c)	Q P
'(a b) '(a b)	Q P
'[] '[]		Q P
'{} '{}		Q P

T?"-- R"


T?"-- S"
'(a . b) Fd	3 SA P  P
'(a . b) Fd	4 SD P  P
'(a . b) FdFdFd	3 SA P  4 SD P  P

T?"setting arguments"T
666 42
11 22 33
\xyz xSyxSz xPyWzW xy+z+^
PPF\

T?"-- T"
"foo"T"bar"

T?"-- U"
'nil	U P
3	U P
"foo"	U P
'foo	U P
'()	U P
'(a b)	U P
'[1 2]	U P
'{}	U P

T?"-- V"


T?"-- W"
'nil	W	'nil	W T
3	W	3	W T
"foo"	W	"foo"	W T
'foo	W	'foo	W T
'()	W	'()	W T
'(a b)	W	'(a b)	W T
'[1 2]	W	'[1 2]	W T
'{}	W	'{}W	W T

T?"-- X"
'nil	X	'nil	X T
3	X	3	X T
"foo"	X	"foo"	X T
'foo	X	'foo	X T
'()	X	'()	X T
{ TODO: '(a b)	X	'(a b)	X T }
'[1 2]	X	'[1 2]	X T
'{}	X	'{}W	X T

T?"-- Y"T
T?"write 42:"	Y P

T?"-- Z"

T?"-- ["

T?"-- \"
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

T?"don't use"
666 42 \
P
666 33 \ 42
PP

T?"-- ]"

T?"-- ^"T
T?"(see \)"T

T?"-- _"


T?"-- `"
`nil P
`a P
`b P
`0 P
`1 P
`2 P
{ TODO: `{} P }
{ TODO: `[] P }

T?"-- {"T
T?"see I"T

T?"-- |"
0 7 	| P
3 8	| P
0 1- 7	| P

T?"-- }"

T?"see {"T

T?"-- ~"
0	~ P
1	~ P
'nil	~ P
'true	~ P
'a	~ P
"foo"	~ P
'[]	- P
'[1]	~ P
'{}	~ P
'{a:33}	~ P

:F . 22 ;
33 11F . .
$n

:F 1-. 22 ;
33 12F . .
$n

:F . 0?{666}{22} ;
33 11F . .
$n

:F d. d?{1-F}{\1n} ;
2n 11F . .
$n

"fib: "
:F dz?{\0}{d1=?{\1}{d 1-F s 2-F +}} ;
0F. 1F. 2F. 3F. 4F. 5F. 6F. 7F. 8F. 9F.
$n
20F.$n
30F.$n
"loop: " 20 { 1-d. "=>" d?] }\ "end"$n
"loop fib: " 20 { 1-d. "=>" dF. d?] }\ "end"$n
$n
"fib: " 0 { d."=>" dF. $n 1+d50<?] }\ "end"

"fours: " 4444 (1111 2222) [_foo _bar `foo@. `bar@. 3333] . .
$n

33 (11 6)[_n `n@. 22 ] . .
$n

"FFFFF:"
:F d. d?{1-F}{\1n} ;
3F.
$n
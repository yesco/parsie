cr"-----------------"cr
:F1$$ $1z?{0^}{$11=?{1^}{$11-F$12-F+^}};

"35F.cr"cr

cr"--- using execution token ---"cr
:G$1z?{0^}{$11=?{1^}{($0$11-)fib($0$12-)fib+^}};

cr"-- 1-3% overhead getting from this--"cr
#^ 32!cr
32@ #fib 'G #: .cr
32@.cr

#^ 64! cr
64@ #__proto__ 32@ #: .cr
"--10% slower with 19 elements--"cr
"--64@ 1#, 2#, 3#, 4#, 5#, 6#, 7#, 8#, 9#, 10#, 11#, 12#, 13#, 14#, 15#, 16#, 17#, 18#, 19#, .cr"
cr"-- skip:  64@ #fib 'G #: .cr"cr
64@.cr

42 11 . (64@ 10)fib . . cr
$.cr

42 11 . (64@ 35)fib . . cr
$.cr

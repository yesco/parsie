cr9e"-- var o= {sum: 0};"
#^ 64!@
64@ #sum 0 #:

cr"-- o.add= function F(a,b) {"cr
"--    var r= a+b;"cr
"--    this.sum += a+b;"cr
"--    return r;"cr
"-- }"cr

:F 3$$ 9e"args:" `2d. @ d. `3d.@d. cr + d #sum `1d.@d. #@ + #sum `1@ #! ^;
:F 3$$ `2 @  `3@ cr + d #sum `1@ #@ + #sum `1@ #! ^;

cr"--obj methods"cr
:M cr"args=" `1@d.  `2@d. cr + d #sum `0@ #@ + #sum `0@ #! ^;
cr9e"TODO: remove 4$$, caller can remove!"cr
:M cr"args=" `1@d.  `2@d. cr + d #sum `0@ #@ + #sum `0@ #! ^;
:N cr"args=" `1@d. cr d #sum `0@ #@ + #sum `0@ #! ^;
cre9"TODO: currently you need to say 2 more than needed"
:Q cr"args=" `1@d. cr d #sum `0@ #@ + #sum `0@ #! ^;
:R cr"args=" `1@d. cr d #sum `0@ #@ + #sum `0@ #! ^;

64@ #add c"M" #:
64@ #plus c"N" #:
64@ #test c"Q" #:
64@ #test2 c"R" #:

cr9e"-- console.log(o);"
64@.cr

cr9e"-- console.log(o.add(11, 22));"cr

"TODO: method lookup so much extra work...."

42 64@ 11 22 F cr

cr9e"42 64@ 11 22 #add 64@ #@ x cr"

"42 64@ 11 22 #add 64@ #@ x cr"

$q
42 (64@ 330000 440000 )add "AFTER=" $. 44 cr

cr9e"TODO: new ops"cr
cr9e"42 (64@ 11 22 )add cr"cr

.cr
9e.cr

9e64@d.cr

cr9e"-- console.log(o.add(1100, 2200));"cr
42 9e"funcall:" 64@ 1100 2200 F
cr.cr9e.

cr9e"-- console.log(o.sum);"cr
#sum 64@ #@ .cr

cr"------------------------------------"cr
42 (64@ 1)plus . 64@. cr
42 (64@ 10)plus . 64@. cr
42 (64@ 100)plus . 64@. cr
42 (64@ 1000)plus . 64@. cr
42 (64@ 10000)plus . 64@. cr
42 (64@ 100000)plus . 64@. cr

cr"===================================="cr
42 (64@ 111111 222222 33333 44444)test . 64@. cr



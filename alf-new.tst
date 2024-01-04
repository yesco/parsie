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

64@ #add c"F" #:

cr9e"-- console.log(o);"
64@.cr

cr9e"-- console.log(o.add(11, 22));"cr

42 64@ 11 22 Fcr
.cr
9e.cr

9e64@d.cr

cr9e"-- console.log(o.add(1100, 2200));"cr
42 9e"funcall:" 64@ 1100 2200 F
cr.cr9e.

cr9e"-- console.log(o.sum);"cr
#sum 64@ #@.cr








cr





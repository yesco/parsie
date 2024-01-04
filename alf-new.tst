"-- var o= {sum: 0};"
#^ 64!@
64@ #sum 0 #:

"-- o.add= function F(a,b) {"cr
"--    var r= a+b;"cr
"--    this.sum += a+b;"cr
"--    return r;"cr
"-- }"cr

:F 3$$ $2 $3 + d #sum $1 #@ + #sum $1 #! 
^ ;

"-- console.log(o);"
64@.cr

"-- console.log(o.add(11, 22));"cr

42 64@ 11 22 F"-->result:".cr
.

64@d.

"sum:" 64@ #sum #@. cr

"-- console.log(o.add(1100, 2200));"cr
42 "funcall:" 64@ 1100 2200 F"-->result:".cr.

"-- console.log(o.sum);"
64@.









cr





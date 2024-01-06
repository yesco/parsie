:C od.od. c=. sc=. cr ;

"--- eq "cr
3 3 C
#a #a C
#nil #nil C
#undef #undef C
#error #error C
c"gurka" c"gurka" C
cr

"--- lt+gt "cr
3 4 C
#a #b C
#nil #undef C
#error #nil C
0 #nil C
3 3*1+ 3 / 4 C
cr

"--- num vs obj"cr
0 #fish C
7 #^ C
3 c"foo" C
cr

"--- num vs nan"cr
0 
7 
3 
cr

cr"-----------------"cr
:X "args:" `0@. $0. `1@. $1d. `2@. $2d. cr + 44. ^;
:X `0@. `1@d. `2@d. + 44. ^;
:Y $0. $1d. $2d. + 44. ^;

cr"--- using execution token ---"cr
#^ 64!cr
64@ #foo 'X #: .cr
64@ #bar 'Y #: .cr

42 11 . (64@ 22 33)foo . . cr
$.cr

42 11 . (64@ 22 33)bar . . cr
$.cr

cr"--- using 'lambda' ---"cr
#^ 64!cr
64@ #foo c"`0@. `1@d. `2@d. + 44. ^" #: .cr
64@ #bar c"$0. $1d. $2d. + 44. ^"    #: .cr

42 11 . (64@ 22 33)foo . . cr
$.cr

42 11 . (64@ 22 33)bar . . cr
$.cr

42 11 . (64@ 22 33 #bar 64@ #@) . . cr
$.cr



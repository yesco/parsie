cr"--- OLD STYLE PARAMETERS---"
$q
:F 2$$ $.cr $1d. $2d. + $. ^;
42 3 4 F$.cr . . cr

$q
:A 3$$ $.cr $1 $2 $3 + + $. ^;
42 1 2 3 A$.cr . . cr

cr"-----------------"cr
:X "args:" `0@. $0. `1@. $1d. `2@. $2d. cr + ;

#^ 64!
64@ #foo c"X" #:
64@.cr
42 11 . (64@ 22 33)foo . . cr


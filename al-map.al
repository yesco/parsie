42 "\v vv+" '(1 2 3 4 5 6 7 8 9 10) MPT PT

42 "\kv k" '(a b c d e f g h i j) MPT PT

42 "\kv kvC" '(a b c d e f g h i j) MPT PT

?"--FOLD"T


"\ab ab+^" '(1 2 3 4 5) 0
\fli lUI{i^} lKI{lAlDRfEFdP^}
P

"\ab ab+^" '(3 4) 10000
\fli ?"call "lWT  lUI{i^}  lKI{?"foo"lA FdP lDFdPRfEFdP^}
P

"\ab ab+^" '(3 4 5) 10000
\fli ?"call "lWT  lUI{i^}  lKI{?"foo="lAWT  1P lDRfEfFdP^}
P

?"---DIY MAPCAR"T

"\a ?aa1+^" '(11 22 33 44)
\fl T?llUI{l^}{lAfE FdP f lD FdP RFdP C FdP^}
P

"1+" '(11 22 33 44)
\fl lUI{l^}{lAfE flDR C^}
P

[\fli lUI{i^} lA flDiR fE^] 'foldr !

'+ '(1 2 3 4 5) 0 _foldr P
'* '(1 2 3 4 5) 1 _foldr P

[\ab ab>I{'nil ^} a a1+bR C^] 'iota !

1 1000 _iota 'list !

'+ 'list@ 0 _foldr P
0 '+ 'list@ 'ri M P

BENCH al	'+ 'list@ 0 _foldr
BENCH C		0 '+ 'list@ 'ri M



'(a b c d e f g h i j k l m n o p q r s t u v w x y z) 'L !

'L @ P T

'{A B C D E F G H I J K L M N O P Q R S T U V W X Y Z} 'O !

'O @ P T

'[A B C D E F G H I J K L M N O P Q R S T U V W X Y Z] 'A !

'A @ P T

25 'L@ N P
25 'O@ N P
25 'A@ N P

'a 'L@ B P
'z 'L@ B P

?"-- nth-1 is faster for Obj/Array as it is clustered, and first match"T

BENCH l-nth-1	1 'A@ N F\
BENCH o-nth-1	1 'L@ N F\
BENCH a-nth-1	1 'O@ N F\

?"-- nth-25 is slower for Obj/Array as it needs to compare"T

BENCH l-nth-25	25 'A@ N F\
BENCH o-nth-25	25 'L@ N F\
BENCH a-nth-25	25 'O@ N F\

?"-- memBer is MUCH slower than nth as it compares and FOLLOWS cdr-ptr"T

BENCH memBer-1	'b 'L@ B F\
BENCH memBer-25	'z 'L@ B F\




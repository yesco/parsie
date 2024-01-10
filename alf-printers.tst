cr"--- .=w space p=no space"cr

42 0 oo.. 9e9e" - " pp cr
c"foo" c"bar" oo.. 9e" - " pp cr
#foo #bar oo.. 9e" - " pp cr
#^ #^ oo.. 9e9e" - " pp cr

cr"--- print foo"cr

'f e 'o e 'o e cr
$"foo" t cr
"  %why it printed a dot?"cr
c"foo" p cr
#foo p cr

cr"--- stringify"cr

42 cs p cr
c"foo" cs p cr
#foo cs p cr
'f ce p 'o ce p 'o ce p cr
#^ cs p "    ==< obj"cr

cr"---formatted strings"cr

42 c%018.2g p cr
1234567890 c%018.18g p cr
c"foo" c%.1s p cr
#foo c%.2s p cr
#^ c% p "    ==< obj"cr

cr"---quoted stringify"cr

42 cq p cr
c"foo" cq p cr
#foo cq p cr
#^ cq p "    ==< obj"cr

cr"---chain appends"cr
c"foo" '- cae #bar cas ': ca%3c 42 ca%018.18g 64 cae 666 cas c"burp" caq p cr

cr
cr


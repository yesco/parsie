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
1.c"foo" dpcr 2.'- cae dpcr 3.#bar cas dpcr 5. c"" 6. ca%3s ">"dp"<"cr 7. 42 8.ca%018.18g dpcr 9. 64 10. cae dpcr 11. 666 12. cas dpcr 13. c"burp" 14. caq dpcr 15. p 16. cr
cr
cr


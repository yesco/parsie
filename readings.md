# references

## TMG

TransMogriFier

## META...

[[http://www.chilton-computing.org.uk/acl/applications/cc/p007.htm#s3p3]]

The construct ADD[2] generates an ADD tree node from the top two items on the stack. The code generator instruction would be of the form:

ADD[-,-] => *1 ' PLUS ' *2 ;
Separating code generation from the building of the parse tree means that quite efficient code can be generated
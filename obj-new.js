?P
function F(n) { this.n= n; } 42;
?P
a= new F(37);
console.log(a);
console.log(a.n);

// direct new foo object

// indirection
//f= foo;
//b= new f(42);
//console.log(b);
//console.log(b.n);

// on global!
//foo(3);
//console.log(n);

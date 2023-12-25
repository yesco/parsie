<pascal.bnf
program foo;

function F(n):integer begin
  if n<=0 then Result:=0;
  else if n=1 then Result:=1;
  else Result:=F(n-1)+F(n-2);
end

begin
  var x: integer = 42;
  var a: integer = 3;
  var r: integer = F(a);
  write("Fib( ");
  write(a);
  write(") = ");
  writeln(r);
  writeln(F(15));
end.

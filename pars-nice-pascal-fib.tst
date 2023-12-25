<pascal.bnf
program foo;

function F(n):integer begin
  if n<=0 then Result:=0;
  else if n=1 then Result:=1;
  else Result:=F(n-1)+F(n-2);
end

begin
  var x: integer = 42;
  var a: integer = 4;
  var r: integer = F(a);
  write("Fib( ");
  write(a);
  write(") = ");
  writeln(r);
  write("FIB: ");
  write(F(0));
  write(F(1));
  write(F(2));
  write(F(3));
  write(F(4));
  write(F(5));
  write(F(6));
  write(F(7));
  write(F(8));
  write(F(9));
  writeln(F(10));

  writeln(F(35));
end.

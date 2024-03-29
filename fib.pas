(* Fibonacci program *)
program Fibonacci;

function F(n:integer):integer begin
  if n<=0 then Result:=0;
  else if n=1 then Result:=1;
  else Result:=F(n-1)+F(n-2);
end

begin
  var x:integer = 42;
  var a:integer = 35;
  var r:integer;

  write("FIB: ");
(*
  writeln(F(9999))
  xy+;3/3;lk3j4t;l43kj
*)
  write(F(0));
  write(F(1)); // mix and match comment
  write(F(2));
  write(F(3));
  write(F(4));
  write(F(5));
  write(F(6));
  write(F(7));
  write(F(8));
  write(F(9));
  writeln(F(10));

  write("Fib( ");
  write(a);
  write(") = ");
  r:= F(a);
  writeln(r);
end.

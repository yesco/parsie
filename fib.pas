program foo;

var n: integer;

function fib(n: integer)
begin
  if n<=0 then fib:=1;
  else fib:= fib(n-1) + fib(n-2);
end

begin
  n:= 7;
  writeln("Fib of ", n, " is ", fib(n));
end.


<pascal.bnf
(* String Test Program *)
program foo;

function G(n):integer begin
  Result:= n+42;
end

function H(n):integer begin
  Result:= 66666+n;
end

begin
  var s: string;
  writeln(E(10000));
  writeln(H(10000));
  writeln(G(10000));
  s:= "foobar";
  write("String=");
  writeln(s);
end.

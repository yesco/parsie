begin
  function zero(): integer begin
    zero:= 0;
  end
  function one(): integer begin
    one:= 1;
  end
  function mone(): integer begin
    mone:= -1;
  end
  function A(k : integer,x1,x2,x3,x4,x5:function: integer): integer begin
    function B(): integer begin
      k:= k-1;
      (* oh, two 'returns' in one! *)
      (* B ref refers to "local" B *)
      B:= A:= A(k,&B,x1,x2,x3,x4);
    end
    if k<=0 then A:= x4+x5 else B();
  end
  // supposewd to pass inf functions
  writeln(1,A(10,one,mone,mone,one,zero));
end.

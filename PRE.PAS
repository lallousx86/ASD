type TPre = record
      str:string[10];
      s1 :char;
end;

var
   f:text;
   pre:array[1..100] of TPre;
   i,j,k:integer;
   s,s1,s2:string;
begin;
  Assign(f,'pre.txt');
  Reset(f);
  i := 1;
  while not(eof(f)) do
  begin;
   readln(f,s);
   if (s[1] = ';') or (s='') then
                                 continue;
   j := pos('=',s);
   pre[i].str := copy(s,1,j-1);
   writeln(s,'/',s1);
   inc(i);
  end;
  Close(f);
end.
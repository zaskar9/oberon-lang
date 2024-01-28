(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Record1;

IMPORT Out;

TYPE
  Date = RECORD day, month, year: INTEGER END;

VAR
  d : Date;

PROCEDURE Test(d : Date);
BEGIN
  d.day := d.day - 1;
  Out.Int(d.year, 0); Out.Ln;
  Out.Int(d.month, 0); Out.Ln;
  Out.Int(d.day, 0); Out.Ln
END Test;

BEGIN
    d.day := 26;
    d.month := 1;
    d.year := 2024;
    Test(d);
    Out.Int(d.year, 0); Out.Ln;
    Out.Int(d.month, 0); Out.Ln;
    Out.Int(d.day, 0); Out.Ln
END Record1.
(*
    CHECK: 2024
    CHECK: 1
    CHECK: 25
    CHECK: 2024
    CHECK: 1
    CHECK: 26
*)
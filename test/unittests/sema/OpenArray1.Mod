(*
  RUN: not %oberon --run %s | filecheck %s
*)
MODULE OpenArray1;
IMPORT Out;

  TYPE
    String = ARRAY 32 OF CHAR;
    StringVec = ARRAY 100, 32 OF CHAR;

  VAR
    a: ARRAY 10 OF String;
    b: StringVec;
    c: ARRAY 100, 32 OF CHAR;

  PROCEDURE proc1(VAR a: ARRAY OF ARRAY 32 OF CHAR);
  BEGIN
    Out.Char("("); Out.Long(LEN(a, 0), 0); Out.String(", "); Out.Long(LEN(a, 1), 0); Out.Char(")"); Out.Ln
  END proc1;

  PROCEDURE proc2(VAR a: ARRAY 100, 32 OF CHAR);
  BEGIN
    proc1(a)
  END proc2;

BEGIN
  proc1(a);
  proc1(b);
  proc1(c);
  proc2(a); (* <-- should trigger a type mismatch! *)
  proc2(b);
  proc2(c)
END OpenArray1.
(*
  CHECK: {{.*}}:30:9:{{.*}}error:{{.*}}type mismatch: incompatible array lengths, found 10, expected 100.
*)
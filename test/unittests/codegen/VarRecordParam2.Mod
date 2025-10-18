(*
  RUN: not %oberon --run %s 2>&1 | filecheck %s
*)
MODULE VarRecordParam2;

  TYPE
    Object = RECORD END;
    Shape = RECORD(Object) area: PROCEDURE(): REAL END;

  VAR
    o: Object;
    s: Shape;

  PROCEDURE Test(VAR o: Object; VAR s: Shape);
  BEGIN
    o := s
  END Test;

BEGIN
    Test(o, s)
END VarRecordParam2.
(*
  CHECK: {{.*}}code 11 (dynamic type mismatch)
*)
(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE RecordExt1;
IMPORT Out;

TYPE R0 = RECORD END;
     R1 = RECORD (R0) v: INTEGER END;

VAR r0: R0;
    r1: R1;

PROCEDURE Test(VAR r: R0);
BEGIN
    IF r IS R1 THEN Out.String("PASS") ELSE Out.String("FAIL") END;
    Out.Ln
END Test;

BEGIN
    Test(r0);
    Test(r1)
END RecordExt1.
(*
    CHECK: FAIL
    CHECK: PASS
*)
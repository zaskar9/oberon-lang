(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE BuiltinLsl1;

IMPORT Out;

PROCEDURE Test;
VAR
    i : INTEGER;
BEGIN
    i := 0FFH;
    Out.Hex(LSL(i, 4)); Out.Ln;
    i := 0H;
    Out.Hex(LSL(i, 0)); Out.Ln;
    i := 0H;
    Out.Hex(LSL(i, 4)); Out.Ln;
    i := -1; (* 0FFFFFFFFH *)
    Out.Hex(LSL(i, 8)); Out.Ln;
END Test;

BEGIN
    Test
END BuiltinLsl1.
(*
    CHECK: 00000FF0
    CHECK: 00000000
    CHECK: 00000000
    CHECK: FFFFFF00
    CHECK-EMPTY
*)
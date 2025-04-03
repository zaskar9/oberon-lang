(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE BuiltinShort2;

IMPORT Out;

PROCEDURE Test;
VAR
    s : INTEGER;
BEGIN
    s := SHORT(123456789);
    Out.Int(s, 0); Out.Ln
END Test;

BEGIN
    Test
END BuiltinShort2.
(*
  CHECK: {{.*}}:12:16:{{.*}}error:{{.*}}constant not valid parameter.
*)
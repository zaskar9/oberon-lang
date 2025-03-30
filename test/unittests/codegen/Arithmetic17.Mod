(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Fails : Current DIV and MOD implementation is not in line with Oberon report.
          Ref. link : https://lists.inf.ethz.ch/pipermail/oberon/2019/013353.html
*)
MODULE Arithmetic17;
IMPORT Out;

PROCEDURE Test();
BEGIN
  Out.Hex(07FFFFFFFH); Out.Ln; (* OK *)
  Out.Hex(08FFFFFFFH); Out.Ln; (* Fails due to sign bit *)
  Out.Hex(0FFFFFFFFH); Out.Ln; (* Fails due to sign bit *)
  Out.Int(5 DIV 3, 0); Out.Ln;
  Out.Int(5 MOD 3, 0); Out.Ln;
  Out.Int((-5) DIV 3, 0); Out.Ln;
  Out.Int((-5) MOD 3, 0); Out.Ln;
  Out.Int(-5 DIV 3, 0); Out.Ln; (* expected : -1,  same as -(5 DIV 3) *)
  Out.Int(-5 MOD 3, 0); Out.Ln; (* expected : -2,  same as -(5 MOD 3) *)
END Test;

BEGIN
    Test
END Arithmetic17.
(*
    CHECK: 7FFFFFFF
    CHECK: 8FFFFFFF
    CHECK: FFFFFFFF
    CHECK: 1
    CHECK: 2
    CHECK: -2
    CHECK: 1
    CHECK: -1
    CHECK: -2
*)

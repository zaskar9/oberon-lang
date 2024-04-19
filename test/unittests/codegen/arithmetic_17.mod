(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
  Fails : Current DIV and MOD implementation is not in line with Oberon report.
          Ref. link : https://lists.inf.ethz.ch/pipermail/oberon/2019/013353.html
*)
MODULE Arithmetic17;

IMPORT Out;

(* From Texts.Mod : JG 21.11.90 / NW 11.7.90 / 24.12.95 / 22.11.10 / 18.11.2014 / 10.1.2019 / AP 15.9.20 Extended Oberon*)
PROCEDURE WriteHex* (x: INTEGER);
VAR i: INTEGER; y, base: INTEGER;
    a: ARRAY 20 OF CHAR;
BEGIN
    i := 0; base := 16;
    REPEAT
        y := x MOD base;
        IF y < 10 THEN
            a[i] := CHR(SHORT(y) + 30H)
        ELSE
            a[i] := CHR(SHORT(y) + 37H)
        END;
        x := x DIV base; INC(i)
    UNTIL i = 8;
    REPEAT DEC(i); Out.Char(a[i]) UNTIL i = 0;
    Out.Ln;
END WriteHex;

PROCEDURE Test();
BEGIN
  WriteHex(07FFFFFFFH); (* OK *)
  WriteHex(08FFFFFFFH); (* Fails due to sign bit *)
  WriteHex(0FFFFFFFFH); (* Fails due to sign bit *)
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

(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE MathTest;
IMPORT Out, M := Math;

CONST
  Tolerance = 1.0E-6; (* Tolerance for comparisons *)

VAR
  errors: INTEGER;

PROCEDURE AssertEqual(expected, actual: REAL; msg: ARRAY OF CHAR);
BEGIN
  IF ABS(expected - actual) > Tolerance THEN
    Out.String("FAIL: "); Out.String(msg);
    Out.String(" expected: "); Out.Real(expected, 15);
    Out.String(", found: "); Out.Real(actual, 15); Out.Ln;
    INC(errors)
  ELSE
    Out.String("PASS: "); Out.String(msg); Out.Ln
  END
END AssertEqual;

PROCEDURE TestSqrtAndPower;
BEGIN
  (* --- Math.sqrt --- *)
  AssertEqual(2.0, M.sqrt(4.0), "sqrt(4.0)");
  (* --- Math.power --- *)
  AssertEqual(8.0, M.power(2.0, 3.0), "power(2, 3)");
  AssertEqual(0.25, M.power(2.0, -2.0), "power(2, -2)");
  AssertEqual(1.0, M.power(0.0, 0.0), "power(0, 0)")
END TestSqrtAndPower;

PROCEDURE TestTrigonometric;
BEGIN
  (* --- Math.sin --- *)
  AssertEqual(0.0, M.sin(0.0), "sin(0)");
  AssertEqual(1.0, M.sin(M.pi / 2), "sin(π/2)");
  AssertEqual(0.0, M.sin(M.pi), "sin(π)");
  (* --- Math.cos --- *)
  AssertEqual(1.0, M.cos(0.0), "cos(0)");
  AssertEqual(0.0, M.cos(M.pi / 2), "cos(π/2)");
  (* --- Math.tan --- *)
  AssertEqual(0.0, M.tan(0.0), "tan(0)");
  (* --- Math.arcsin --- *)
  AssertEqual(M.pi / 2, M.arcsin(1.0), "arcsin(1)");
  AssertEqual(0.0, M.arcsin(0.0), "arcsin(0)");
  (* --- Math.arccos --- *)
  AssertEqual(0.0, M.arccos(1.0), "arccos(1)");
  AssertEqual(M.pi / 2, M.arccos(0.0), "arccos(0)");
  (* --- Math.arctan --- *)
  AssertEqual(M.pi / 4, M.arctan(1.0), "arctan(1)");
  AssertEqual(-M.pi / 4, M.arctan(-1.0), "arctan(-1)");
  (* --- Math.arctan2 --- *)
  AssertEqual(M.pi / 4, M.arctan2(1.0, 1.0), "arctan2(1,1)");
  AssertEqual(3 * M.pi / 4, M.arctan2(1.0, -1.0), "arctan2(1,-1)")
END TestTrigonometric;

PROCEDURE TestHyperbolic;
BEGIN
  (* --- Math.sinh --- *)
  AssertEqual((M.exp(1.0) - M.exp(-1.0)) / 2.0, M.sinh(1.0), "sinh(1)");
  (* --- Math.cosh --- *)
  AssertEqual((M.exp(1.0) + M.exp(-1.0)) / 2.0, M.cosh(1.0), "cosh(1)");
  (* --- Math.tanh --- *)
  AssertEqual((M.exp(1.0) - M.exp(-1.0)) / (M.exp(1.0) + M.exp(-1.0)), M.tanh(1.0), "tanh(1)");
  (* --- Math.arcsinh --- *)
  AssertEqual(0.881373, M.arcsinh(1.0), "arcsinh(1.0)"); (* Expected ≈ ln(1 + sqrt(2)) ≈ 0.881373 *)
  (* --- Math.arccosh --- *)
  AssertEqual(1.316958, M.arccosh(2.0), "arccosh(2.0)"); (* Expected ≈ ln(2 + sqrt(3)) ≈ 1.31696 *)
  (* --- Math.arctanh --- *)
  AssertEqual(0.549306, M.arctanh(0.5), "arctanh(0.5)") (* Expected ≈ 0.549306 *)
END TestHyperbolic;

PROCEDURE TestConversions;
BEGIN
  (* --- Math.round --- *)
  AssertEqual(2.0, M.round(2.4), "round(2.4)");
  AssertEqual(3.0, M.round(2.5), "round(2.5)");
  AssertEqual(-2.0, M.round(-2.4), "round(-2.4)");
  AssertEqual(-3.0, M.round(-2.5), "round(-2.5)");
  (* --- Math.entier --- *)
  AssertEqual(2.0, M.entier(2.9), "entier(2.9)");
  AssertEqual(-3.0, M.entier(-2.1), "entier(-2.1)");
  AssertEqual(0.0, M.entier(0.0), "entier(0.0)");
  (* --- Math.real --- *)
  AssertEqual(42.0, M.real(42), "real(42)")
END TestConversions;

PROCEDURE TestExponentialAndLog;
BEGIN
  (* --- Math.exp --- *)
  AssertEqual(M.e, M.exp(1.0), "exp(1.0)");
  AssertEqual(1.0, M.exp(0.0), "exp(0.0)");
  (* --- Math.ln --- *)
  AssertEqual(1.0, M.ln(M.e), "ln(e)");
  (* --- Math.log --- *)
  AssertEqual(2.0, M.log(100.0, 10.0), "log(100, 10)");
  AssertEqual(0.0, M.log(1.0, 2.0), "log(1, 2)");
  (* --- identities --- *)
  AssertEqual(2.5, M.ln(M.exp(2.5)), "ln(exp(2.5))");
  AssertEqual(2.5, M.exp(M.ln(2.5)), "exp(ln(2.5))")
END TestExponentialAndLog;

BEGIN
  errors := 0;
  TestSqrtAndPower;
  TestTrigonometric;
  TestHyperbolic;
  TestConversions;
  TestExponentialAndLog;
  ASSERT(errors = 0)
END MathTest.

(*

PROCEDURE TestPowerLogIdentities;
VAR
  x, y, base, result: REAL;
  lx, ly, lbase, lresult: LONGREAL;
BEGIN
  Out.String("Testing Power and Logarithm Identities"); Out.Ln;

  (* log(power(x, b), b) = x *)
  x := 2.5; base := 5.0;
  result := Math.log(Math.power(x, base), base);
  AssertRealEqual(x, result, "log(power(2.5, 5), 5) = 2.5");

  lx := 2.5D0; lbase := 5.0D0;
  lresult := MathL.log(MathL.power(lx, lbase), lbase);
  AssertLongRealEqual(lx, lresult, "log(power(2.5, 5), 5) LONGREAL");

  (* power(log(x, b), b) = x *)
  x := 9.0; base := 3.0;
  result := Math.power(Math.log(x, base), base);
  AssertRealEqual(x, result, "power(log(9, 3), 3) = 9");

  lx := 9.0D0; lbase := 3.0D0;
  lresult := MathL.power(MathL.log(lx, lbase), lbase);
  AssertLongRealEqual(lx, lresult, "power(log(9, 3), 3) LONGREAL");

  (* log(x, b) = ln(x) / ln(b) *)
  x := 42.0; base := 2.0;
  result := Math.log(x, base);
  AssertRealEqual(Math.ln(x) / Math.ln(base), result, "log(42, 2) = ln(42)/ln(2)");

  lx := 42.0D0; lbase := 2.0D0;
  lresult := MathL.log(lx, lbase);
  AssertLongRealEqual(MathL.ln(lx) / MathL.ln(lbase), lresult, "log(42, 2) LONGREAL");

  (* power(x + y, b) = b^x * b^y *)
  x := 1.5; y := 2.0; base := 2.0;
  result := Math.power(x + y, base);
  AssertRealEqual(Math.power(x, base) * Math.power(y, base), result, "power(1.5 + 2.0, 2) = power(1.5,2)*power(2.0,2)");

  (* power(x, b * c) = power(x, b)^c *)
  x := 2.0; base := 3.0; y := 2.0;  (* treat y as 'c' *)
  result := Math.power(x, base * y);
  AssertRealEqual(Math.power(x, base) * Math.power(x, base), result, "power(2, 6) = power(2,3)^2");

END TestPowerLogIdentities;


*)

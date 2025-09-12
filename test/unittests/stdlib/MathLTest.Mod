(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE MathLTest;
IMPORT Out, M := MathL;

CONST
  Tolerance = 1.0E-15; (* Tolerance for comparisons *)

VAR
  errors: INTEGER;

PROCEDURE AssertEqual(expected, actual: LONGREAL; msg: ARRAY OF CHAR);
BEGIN
  IF ABS(expected - actual) > Tolerance THEN
    Out.String("FAIL: "); Out.String(msg);
    Out.String(" expected: "); Out.LongReal(expected, 23);
    Out.String(", found: "); Out.LongReal(actual, 23); Out.Ln;
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
  AssertEqual(16.0, M.power(4.0, 2.0), "power(4, 2)");
  AssertEqual(0.0625, M.power(4.0, -2.0), "power(4, -2)");
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
  AssertEqual(0.8813735870195430E64 * 1.0E-64, M.arcsinh(1.0), "arcsinh(1.0)");
  (* --- Math.arccosh --- *)
  AssertEqual(1.3169578969248170E64 * 1.0E-64, M.arccosh(2.0), "arccosh(2.0)");
  (* --- Math.arctanh --- *)
  AssertEqual(0.5493061443340550E64 * 1.0E-64, M.arctanh(0.5), "arctanh(0.5)")
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
  AssertEqual(2.0, M.log(9.0, 3.0), "log(9, 3)");
  AssertEqual(0.0, M.log(1.0, 5.0), "log(1, 5)");
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
END MathLTest.

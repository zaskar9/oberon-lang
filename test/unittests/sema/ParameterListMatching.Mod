(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon -fenable-varargs --run %s | filecheck %s
*)
MODULE ParameterListMatching;

  TYPE
    Variadic = PROCEDURE(x, y: INTEGER; ...);
    RealFun = PROCEDURE(x: REAL): REAL;
    Real2IntFun = PROCEDURE(x: REAL): INTEGER;
    Int2RealFun = PROCEDURE(x: INTEGER): REAL;
    VarParams = PROCEDURE(x: INTEGER; VAR y: INTEGER): INTEGER;

  VAR
    var: Variadic;
    rf: RealFun;
    i2r: Int2RealFun;
    r2i: Real2IntFun;
    vp : VarParams;

  PROCEDURE plot(x, y: INTEGER);
  END plot;

  PROCEDURE sum(x, y: INTEGER): INTEGER;
  BEGIN
    RETURN x + y
  END sum;

  PROCEDURE sqrt(x: REAL): REAL;
  BEGIN
    RETURN 0.0
  END sqrt;

BEGIN
  var := plot;
  rf := sum;
  i2r := sqrt;
  r2i := sqrt;
  vp := sum
END ParameterListMatching.
(*
  CHECK: {{.*}}:34:10:{{.*}}error:{{.*}}type mismatch: procedure types with variadic arguments are incompatible.
  CHECK: {{.*}}:35:9:{{.*}}error:{{.*}}type mismatch: procedure types have different number of parameters.
  CHECK: {{.*}}:36:10:{{.*}}error:{{.*}}type mismatch: procedure types have different parameter types.
  CHECK: {{.*}}:37:10:{{.*}}error:{{.*}}type mismatch: procedure types have different return types.
  CHECK: {{.*}}:38:9:{{.*}}error:{{.*}}type mismatch: procedure types have incompatible variable parameters.
*)
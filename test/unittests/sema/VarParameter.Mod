(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s 2>&1 | filecheck %s
*)
MODULE VarParameter;

TYPE Point2D = RECORD x, y: INTEGER END;
     Vector = ARRAY 10 OF INTEGER;

VAR p: Point2D;
    v: Vector;

(* O07.9.1: If a value parameter is structured (of array or record type),
 * no assignment to it or to its elements are permitted.
 *)
PROCEDURE Test1(p: Point2D);
BEGIN
    p.x := 0;
    p.y := 0
END Test1;

(* O07.9.1: If a value parameter is structured (of array or record type),
 * no assignment to it or to its elements are permitted.
 *)
PROCEDURE Test2(v: Vector);
BEGIN
    v[0] := 0
END Test2;

BEGIN
    Test1(p);
    Test2(v)
END VarParameter.
(*
  CHECK: {{.*}}:17:5:{{.*}}error:{{.*}}cannot assign to a non-variable structured parameter.
  CHECK: {{.*}}:18:5:{{.*}}error:{{.*}}cannot assign to a non-variable structured parameter.
  CHECK: {{.*}}:26:5:{{.*}}error:{{.*}}cannot assign to a non-variable structured parameter.
*)
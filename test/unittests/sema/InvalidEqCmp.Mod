(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE InvalidEqCmp;

VAR string: ARRAY 32 OF CHAR;
    vector: ARRAY 100 OF INTEGER;
    matrix: ARRAY 10, 10 OF CHAR;

BEGIN
    ASSERT(string # string);
    ASSERT(string = string);
    ASSERT(string = vector);
    ASSERT(vector = vector);
    ASSERT(vector = matrix);
    ASSERT(matrix = matrix)
END InvalidEqCmp.
(*
  CHECK: {{.*}}:13:12:{{.*}}error:{{.*}}comparison operator requires one-dimensional character array arguments.
  CHECK: {{.*}}:14:12:{{.*}}error:{{.*}}comparison operator requires one-dimensional character array arguments.
  CHECK: {{.*}}:15:12:{{.*}}error:{{.*}}comparison operator requires one-dimensional character array arguments.
  CHECK: {{.*}}:16:12:{{.*}}error:{{.*}}comparison operator requires one-dimensional character array arguments.
*)
(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE NumericInclusion2;

VAR
  s: SHORTINT;
  i: INTEGER;
  l: LONGINT;
  f: REAL;
  d: LONGREAL;

BEGIN
  s := i; s := l; s := f; s := d;
  i := l; i := f; i := d;
  l := f; l := d;
  f := d
END NumericInclusion2.
(*
  CHECK: {{.*}}:14:8:{{.*}}error:{{.*}}type mismatch: converting from INTEGER to SHORTINT may lose data.
  CHECK: {{.*}}:14:16:{{.*}}error:{{.*}}type mismatch: converting from LONGINT to SHORTINT may lose data.
  CHECK: {{.*}}:14:24:{{.*}}error:{{.*}}type mismatch: type REAL is not included in type SHORTINT.
  CHECK: {{.*}}:14:32:{{.*}}error:{{.*}}type mismatch: type LONGREAL is not included in type SHORTINT.
  CHECK: {{.*}}:15:8:{{.*}}error:{{.*}}type mismatch: converting from LONGINT to INTEGER may lose data.
  CHECK: {{.*}}:15:16:{{.*}}error:{{.*}}type mismatch: type REAL is not included in type INTEGER.
  CHECK: {{.*}}:15:24:{{.*}}error:{{.*}}type mismatch: type LONGREAL is not included in type INTEGER.
  CHECK: {{.*}}:16:8:{{.*}}error:{{.*}}type mismatch: type REAL is not included in type LONGINT.
  CHECK: {{.*}}:16:16:{{.*}}error:{{.*}}type mismatch: type LONGREAL is not included in type LONGINT.
  CHECK: {{.*}}:17:8:{{.*}}error:{{.*}}type mismatch: converting from LONGREAL to REAL may lose data.
*)
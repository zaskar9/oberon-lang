(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE VarParameter3;
  IMPORT In, Out;

  TYPE
    String = ARRAY 32 OF CHAR;
    Person = RECORD
      first, last, street, city, country: String;
      zip: INTEGER;
      phone, mail: String
    END;

  VAR
    p: Person;

  PROCEDURE init(p: Person);
  BEGIN
    In.String(p.first);
    In.String(p.last)
  END init;

BEGIN
  In.Open;
  init(p);
  Out.String(p.last); Out.String(", "); Out.String(p.first); Out.Ln
END VarParameter3.
(*
  CHECK: {{.*}}:20:15:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a non-variable structured parameter by reference.
  CHECK: {{.*}}:21:15:{{.*}}error:{{.*}}illegal actual parameter: cannot pass a non-variable structured parameter by reference.
*)
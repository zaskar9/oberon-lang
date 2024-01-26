(*
  RUN: %oberon --run %s | filecheck %s
*)
MODULE Record2;

TYPE
  Date = RECORD day, month, year: INTEGER END;

VAR
  d : Date;

PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

PROCEDURE Test(VAR d : Date);
BEGIN
  d.day := d.day - 1;
  printf("%04d.%02d.%02d\n", d.year, d.month, d.day)
END Test;

BEGIN
    d.day := 26;
    d.month := 1;
    d.year := 2024;
    Test(d);
    printf("%04d.%02d.%02d\n", d.year, d.month, d.day)
END Record2.
(*
    CHECK: 2024.01.25
    CHECK: 2024.01.25
*)
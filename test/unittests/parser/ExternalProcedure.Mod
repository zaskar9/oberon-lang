(*
  RUN: not %oberon -fenable-extern -fenable-varargs --run %s 2>&1 | filecheck %s
*)
MODULE ExternalProcedure;

    PROCEDURE [ "C" ] PrintF(format: ARRAY OF CHAR; ...); EXTERNAL [ "printf" ];

    PROCEDURE WriteLn(str: ARRAY OF CHAR);
    BEGIN
        PrintF("%s\n", str)
    END WriteLn;

BEGIN
    WriteLn("Hello World!")
END ExternalProcedure.
(*
  CHECK: Hello World!
*)
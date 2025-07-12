(*
  RUN: %oberon -fsanitize=all -fenable-extern -fenable-varargs --run %s | filecheck %s
*)
MODULE ExternalProcedure;

    PROCEDURE [ "C" ] PrintF(format: ARRAY OF CHAR; ...); EXTERNAL [ "printf" ];

    PROCEDURE WriteLn*(str: ARRAY OF CHAR);
    BEGIN
        PrintF("%s\n", str)
    END WriteLn;

BEGIN
    WriteLn("Hello World!")
END ExternalProcedure.
(*
  CHECK: Hello World!
*)
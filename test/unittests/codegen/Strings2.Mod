(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s
*)
MODULE Strings2;

VAR str: ARRAY 32 OF CHAR;

PROCEDURE StrEq(str1, str2: ARRAY OF CHAR): BOOLEAN;
BEGIN
    RETURN str1 = str2
END StrEq;

BEGIN
    str := "Hello World!";
    ASSERT(str = str);
    ASSERT(~(str # str));
    ASSERT(StrEq(str, "Hello World!"))
END Strings2.
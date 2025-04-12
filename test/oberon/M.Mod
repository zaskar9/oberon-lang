MODULE M;

CONST
    CCHAR* = 041X;
    CBYTE* = 255;
    CINT* = 100;
    CLINT* = 0FFFFFFFFFFFFFFFFH;
    CREAL* = 1.1;
    CREALMAX* = 1.7976931348623157E+308;
    CBOOL* = TRUE;
    CSET* = {0, 31};
    CSTR* = "testing123";

VAR
    C* : CHAR;
    X* : BYTE;
    I* : INTEGER;
    LI* : LONGINT;
    R* : REAL;
    LR* : LONGREAL;
    B* : BOOLEAN;
    S* : SET;
    STR* : ARRAY 25 OF CHAR;

BEGIN
    C := CCHAR;
    X := CBYTE;
    I := CINT;
    LI := CLINT;
    R := CREAL;
    LR := CREALMAX;
    B := CBOOL;
    S := CSET;
    STR := CSTR
END M.
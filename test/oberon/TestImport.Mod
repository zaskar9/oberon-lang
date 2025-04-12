MODULE TestImport;
IMPORT M, Out;

VAR
    c : CHAR;
    x : BYTE;
    i : INTEGER;
    l : LONGINT;
    r : REAL;
    lr : LONGREAL;
    b : BOOLEAN;
    s : SET;
    str : ARRAY 25 OF CHAR;
BEGIN
    (* Test CONST *)
    c := M.CCHAR;
    Out.Char(c); Out.Ln;
    i := M.CINT;
    Out.Int(i, 0); Out.Ln;
    l := M.CLINT;
    Out.Long(l, 0); Out.Ln;
    r := M.CREAL;
    Out.Real(r, 9); Out.Ln;
    lr := M.CREAL;
    Out.LongReal(lr, 17); Out.Ln;
    lr := M.CREALMAX;
    Out.Real(SHORT(lr), 9); Out.Ln;
    lr := M.CREALMAX;
    Out.LongReal(lr, 17); Out.Ln;
    b := M.CBOOL;
    Out.Int(ORD(b), 0); Out.Ln;
    s := M.CSET;
    Out.Set(s); Out.Ln;
    str := M.CSTR;
    Out.String(str); Out.Ln;
    (* Test VAR *)
    c := M.C; (* This gives segmentation fault *)
    Out.Char(c); Out.Ln
END TestImport.
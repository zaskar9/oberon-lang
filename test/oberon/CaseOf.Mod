MODULE CaseOf;
IMPORT Out;

CONST
    EmptySet = {};
    FullSet = { 0 .. 31 };
    Set = { 8, 10 .. 12, 15 };
    Union = { 0 .. 5 } + { 3, 5, 7 .. 9 };
    (* chars = { "A" .. "Z", "a" .. "z" }; *)

TYPE
    ObjectDesc = RECORD id: INTEGER END;
    Object = POINTER TO ObjectDesc;
    PersonDesc = RECORD(* ObjectDesc *) age: INTEGER; END;
    Person = POINTER TO PersonDesc;
    Vector = ARRAY 10 OF INTEGER;

PROCEDURE CharacterClass(ch: CHAR);
BEGIN
  CASE ch OF
      9 .. 1 : Out.String("Test"); Out.Ln;
    | "A" .. "Z": Out.String("uppercase letter")
    | "a" .. "z": Out.String("lowercase letter")
    | "0" .. ch: Out.String("digit")
    | ch: Out.String("identity")
    | Object: Out.String("object")
    | ObjectDesc: Out.String("record")
    | Vector: Out.String("vector");;;;;
  ELSE
    Out.String("special character");
  END
END CharacterClass;

END CaseOf.
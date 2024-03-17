MODULE TypeType;
IMPORT Out;

TYPE
    String = ARRAY 256 OF CHAR;
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
        val: INTEGER;
        prev, next: Node
    END;

VAR p: Node;
    v: INTEGER;

BEGIN
    Out.Long(SIZE(INTEGER), 0); Out.Ln;
    Out.Long(SIZE(String), 0); Out.Ln;
    Out.Long(SIZE(Node), 0); Out.Ln;
    Out.Long(SIZE(NodeDesc), 0); Out.Ln
END TypeType.
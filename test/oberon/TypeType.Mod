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
    Out.Int(SIZE(INTEGER), 0); Out.Ln;
    Out.Int(SIZE(String), 0); Out.Ln;
    Out.Int(SIZE(Node), 0); Out.Ln;
    Out.Int(SIZE(NodeDesc), 0); Out.Ln
    (* v := p(Node).val *)
END TypeType.
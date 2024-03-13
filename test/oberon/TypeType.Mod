MODULE TypeType;
IMPORT Out;

TYPE
    String = ARRAY 256 OF CHAR;
    Node = POINTER TO NodeDesc;
    NodeDesc = RECORD
        val: INTEGER;
        prev, next: Node
    END;

BEGIN
    Out.Int(SIZE(INTEGER), 0);
    Out.Int(SIZE(String), 0);
    Out.Int(SIZE(Node), 0);
    Out.Int(SIZE(NodeDesc), 0)
END TypeType.
MODULE LinkedList;
IMPORT Out;

TYPE Node = POINTER TO NodeDesc;
     NodeDesc = RECORD
                    value: INTEGER;
                    next: Node
                END;
     List* = RECORD
                first, last: Node
            END;

VAR list: List;
    i: INTEGER;

PROCEDURE init(VAR l: List);
BEGIN
    l.first := NIL;
    l.last := l.first
END init;

PROCEDURE insert(VAR l: List; val: INTEGER);
VAR node: Node;
BEGIN
    NEW(node);
    node.value := val;
    node.next := NIL;
    IF l.last # NIL THEN
        l.last.next := node;
        l.last := node
    ELSE
        l.first := node;
        l.last := node
    END
END insert;

PROCEDURE print(l: List);
VAR current: Node;
BEGIN
    Out.String("[");
    current := l.first;
    WHILE current # NIL DO
        Out.Int(current.value, 0);
        IF current # list.last THEN Out.String(", ") END;
        current := current.next
    END;
    Out.String("]")
END print;

BEGIN
    init(list);
    FOR i := 0 TO 10 DO
        insert(list, i)
    END;
    print(list); Out.Ln
END LinkedList.
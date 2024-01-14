MODULE DoublyLinkedList;
IMPORT Out;

TYPE
  Node = POINTER TO NodeDesc;
  NodeDesc = RECORD
    value: INTEGER;
    prev, next: Node
  END;
  List* = RECORD
    head, tail: Node
  END;

VAR
  list: List;

PROCEDURE append(VAR l: List; value: INTEGER);
  VAR
    newNode: Node;
  BEGIN
    NEW(newNode);
    newNode.value := value;

    IF l.tail = NIL THEN
      (* If the list is empty, set the new node as both head and tail *)
      l.head := newNode;
      l.tail := newNode
    ELSE
      (* Append the new node to the end of the list *)
      newNode.prev := l.tail;
      l.tail.next := newNode;
      l.tail := newNode
    END
  END append;

PROCEDURE print(l: List);
  VAR
    currentNode: Node;
    first: BOOLEAN;
  BEGIN
    currentNode := l.head;
    Out.String("[");
    first := TRUE;
    WHILE currentNode # NIL DO
      IF ~first THEN
        Out.String(", ")
      END;
      (* Print the value of the current node *)
      Out.Int(currentNode.value);
      (* Move to the next node *)
      currentNode := currentNode.next;
      first := FALSE
    END;
    Out.String("]"); Out.Ln
  END print;

BEGIN
  list.head := NIL;
  list.tail := list.head;

  (* Insert values into the doubly linked list *)
  append(list, 10);
  append(list, 20);
  append(list, 30);
  append(list, 40);

  (* Display the doubly linked list *)
  print(list)
END DoublyLinkedList.

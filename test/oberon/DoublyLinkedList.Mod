MODULE DoublyLinkedList;
IMPORT Out;

TYPE
  Node = RECORD
    value: INTEGER;
    prev, next: POINTER TO Node
  END;

VAR
  head, tail: POINTER TO Node;

PROCEDURE InsertAtEnd(value: INTEGER);
  VAR
    newNode: POINTER TO Node;
  BEGIN
    NEW(newNode);
    newNode.value := value;

    IF tail = NIL THEN
      (* If the list is empty, set the new node as both head and tail *)
      head := newNode;
      tail := newNode
    ELSE
      (* Append the new node to the end of the list *)
      newNode.prev := tail;
      tail.next := newNode;
      tail := newNode
    END
  END InsertAtEnd;

PROCEDURE DisplayList;
  VAR
    currentNode: POINTER TO Node;
  BEGIN
    currentNode := head;
    WHILE currentNode # NIL DO
      (* Print the value of the current node *)
      Out.Int(currentNode.value); Out.String(" ");
      (* Move to the next node *)
      currentNode := currentNode.next
    END;
    Out.Ln
  END DisplayList;

BEGIN
  head := NIL;
  tail := NIL;

  (* Insert values into the doubly linked list *)
  InsertAtEnd(10);
  InsertAtEnd(20);
  InsertAtEnd(30);
  InsertAtEnd(40);

  (* Display the doubly linked list *)
  DisplayList
END DoublyLinkedList.

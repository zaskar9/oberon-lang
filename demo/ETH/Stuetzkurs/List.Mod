MODULE List;
IMPORT Out;

  TYPE
    Object* = POINTER TO ObjectDesc;
    ObjectDesc* = RECORD
      compare*: PROCEDURE(this, other: Object): INTEGER;
      print*: PROCEDURE(this: Object)
    END;
    Element = POINTER TO ElementDesc;
    ElementDesc = RECORD
      value: Object;
      next: Element
    END;
    List* = POINTER TO ListDesc;
    ListDesc* = RECORD
      first, last: Element;
      insert*: PROCEDURE(this: List; o: Object);
      delete*: PROCEDURE(this: List; o: Object);
      print*: PROCEDURE(this: List)
    END;

    (* Developer-defined types. *)
    Integer* = POINTER TO IntegerDesc;
    IntegerDesc* = RECORD (ObjectDesc)
      value*: INTEGER
    END;

  VAR
    i: INTEGER;
    value: Integer;
    l: List;

  PROCEDURE compareInt(this, other: Object): INTEGER;
  BEGIN
    RETURN this(Integer).value - other(Integer).value
  END compareInt;

  PROCEDURE printInt(this: Object);
  BEGIN
    Out.Int(this(Integer).value, 0)
  END printInt;

  PROCEDURE newInt(value: INTEGER): Integer;
  VAR res: Integer;
  BEGIN
    NEW(res);
    res.value := value;
    res.compare := compareInt;
    res.print := printInt;
    RETURN res
  END newInt;

  PROCEDURE insert(l: List; o: Object);
  VAR e: Element;
  BEGIN
    NEW(e);
    e.value := o;
    e.next := NIL;
    IF l.first # NIL THEN
      l.last.next := e
    ELSE
      l.first := e
    END;
    l.last := e
  END insert;

  PROCEDURE delete(l: List; o: Object);
  VAR cur, prev: Element;
      val: Object;
  BEGIN
    cur := l.first;
    prev := NIL;
    WHILE cur # NIL DO
      val := cur.value;
      IF val.compare(val, o) = 0 THEN
        IF prev # NIL THEN
          prev.next := cur.next;
          IF cur = l.last THEN
            l.last := prev
          END
        ELSE
          l.first := cur.next
        END;
        DISPOSE(cur.value);
        DISPOSE(cur);
        EXIT
      ELSE
        prev := cur;
        cur := cur.next
      END
    END
  END delete;

  PROCEDURE print(l: List);
  VAR cur: Element;
      val: Object;
  BEGIN
    cur := l.first;
    Out.Char("[");
    WHILE cur # NIL DO
      val := cur.value;
      val.print(val);
      cur := cur.next;
      IF (cur # NIL) THEN Out.Char(",") END
    END;
    Out.Char("]")
  END print;

  PROCEDURE newList*(): List;
  VAR l: List;
  BEGIN
    NEW(l);
    l.first := NIL; l.last := NIL;
    l.insert := insert;
    l.delete := delete;
    l.print := print;
    RETURN l
  END newList;

  PROCEDURE deleteList*(l: List);
  VAR cur, tmp: Element;
  BEGIN
    cur := l.first;
    WHILE cur # NIL DO
      tmp := cur;
      cur := cur.next;
      DISPOSE(tmp.value);
      DISPOSE(tmp)
    END;
    DISPOSE(l)
  END deleteList;

BEGIN
  l := newList();
  l.print(l); Out.Ln;
  FOR i := 0 TO 9 DO
    l.insert(l, newInt(i))
  END;
  l.print(l); Out.Ln;
  l.delete(l, newInt(0));
  l.delete(l, newInt(5));
  l.delete(l, newInt(9));
  l.print(l); Out.Ln;
  deleteList(l)
END List.
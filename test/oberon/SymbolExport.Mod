MODULE SymbolExport;
IMPORT Oberon;

CONST BOOLFALSE* = FALSE;
      BOOLTRUE* = TRUE;
      CHARMIN* = 0X;
      CHARMAX* = 0FFX;
      SHORTMIN* = -32768;
      SHORTMAX* = 32767;
      INTMIN* = -2147483648;
      INTMAX* = 2147483647;
      LONGMIN* = -9223372036854775808;
      LONGMAX* = 9223372036854775807;
      FLOATMIN* = 1.17549435E-38;
      FLOATMAX* = 3.40282347E+38;
      DOUBLEMIN* = 2.22507385850720138E-308;
      DOUBLEMAX* = 1.79769313486231571E+308;
      STRING* = "Hello World";
      SETMIN* = {};
      SETMAX* = { 0 .. 31 };

TYPE Vector* = ARRAY 10 OF INTEGER;
     Matrix* = ARRAY 10, 10 OF INTEGER;
     (*Object* = RECORD END;*)
     Point2d* = RECORD
        x*, y*: INTEGER
     END;
     (*Point3d* = RECORD (Point2d)
        z: INTEGER
     END;*)
     (*Shape* = RECORD
        area: PROCEDURE (): LONGREAL
     END;*)
     Circle* = RECORD
        center*: Point2d;
        radius: REAL
     END;
     Triangle* = RECORD
        a, b, c: Point2d
     END;
     Polygon* = RECORD
        points*: ARRAY 256 OF Point2d
     END;
     Polygons* = ARRAY 1024 OF Polygon;
     Node* = POINTER TO NodeDesc;
     NodeDesc* = RECORD
        val*: INTEGER;
        next*: Node
     END;
     (*Elem* = POINTER TO RECORD val*: INTEGER; next*: Elem END;*)
     BinTreeNode* = RECORD val*: INTEGER; left*, right*: POINTER TO BinTreeNode END;

VAR s: SHORTINT;
    i: INTEGER;
    l: LONGINT;
    f: REAL;
    d: LONGREAL;

PROCEDURE SetTime*(time: Oberon.TimeSpec);
END SetTime;

PROCEDURE Swap*(VAR left, right: BinTreeNode);
END Swap;

BEGIN
    s := SHORTMAX;
    i := SHORTMAX * SHORTMAX;
    l := INTMAX * INTMAX;
    f := FLOATMAX;
    d := FLOATMAX * FLOATMAX
END SymbolExport.
MODULE TestExport;

CONST Len = 10;

TYPE
     Object* = POINTER TO ObjectDesc;
     ObjectDesc* = RECORD END;
     Node* = POINTER TO NodeDesc;
     NodeDesc* = RECORD (ObjectDesc)
        val*: INTEGER;
        next: Node
     END;
     NotExported = RECORD END;
     Exported* = POINTER TO NotExported;
     LocalType = ARRAY Len OF NotExported;

    PROCEDURE DoExported*(VAR list: LocalType);
    END DoExported;

END TestExport.
MODULE TestExport;

TYPE
     Object* = POINTER TO ObjectDesc;
     ObjectDesc* = RECORD END;
     Node* = POINTER TO NodeDesc;
     NodeDesc* = RECORD (ObjectDesc)
        val*: INTEGER;
        next*: Node
     END;

END TestExport.
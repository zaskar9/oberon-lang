MODULE TypeCycles;

TYPE Invalid = Invalid;
     Infinite = RECORD field: Infinite END;
     Loop = ARRAY 10 OF Loop;
     Recursive = Mutual;
     Mutual = Recursive;
     Foo = POINTER TO Bar;
     Bar = Foo;

END TypeCycles.
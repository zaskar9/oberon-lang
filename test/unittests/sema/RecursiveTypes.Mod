(*
  RUN: not %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --std=Oberon07 --run %s 2>&1 | filecheck %s
*)
MODULE RecursiveTypes;

TYPE Invalid = Invalid;
     Infinite = RECORD field: Infinite END;
     Loop = ARRAY 10 OF Loop;
     Recursive = Mutual;
     Mutual = Recursive;
     Foo = POINTER TO Bar;
     Bar = Foo;

END RecursiveTypes.
(*
  CHECK: {{.*}}:6:16:{{.*}}error:{{.*}}undefined type: Invalid.
  CHECK: {{.*}}:7:31:{{.*}}error:{{.*}}undefined type: Infinite.
  CHECK: {{.*}}:8:25:{{.*}}error:{{.*}}undefined type: Loop.
  CHECK: {{.*}}:9:18:{{.*}}error:{{.*}}undefined type: Mutual.
  CHECK: {{.*}}:10:15:{{.*}}error:{{.*}}undefined type: Recursive.
  CHECK: {{.*}}:11:23:{{.*}}error:{{.*}}pointer base type must be a record type.
*)
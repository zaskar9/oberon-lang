(*
  RUN: not %oberon --std=Oberon2 --run %s | filecheck %s
*)
MODULE DynamicArray1;

  TYPE
    Array = ARRAY 10 OF ARRAY OF CHAR;
    Record = RECORD a: ARRAY OF CHAR END;
    Procedure = PROCEDURE(a: ARRAY 20 OF ARRAY OF CHAR);

  VAR
    buf1: POINTER TO ARRAY 20 OF ARRAY OF CHAR;
    buf2: ARRAY OF CHAR;

  PROCEDURE proc1(a: ARRAY 20 OF ARRAY OF CHAR);
  END proc1;

END DynamicArray1.
(*
  CHECK: {{.*}}:7:25:{{.*}}error:{{.*}}open array type cannot be used as element type of non-open array type.
  CHECK: {{.*}}:8:21:{{.*}}error:{{.*}}open array type cannot be used as record field type.
  CHECK: {{.*}}:9:42:{{.*}}error:{{.*}}open array type cannot be used as element type of non-open array type.
  CHECK: {{.*}}:12:34:{{.*}}error:{{.*}}open array type cannot be used as element type of non-open array type.
  CHECK: {{.*}}:13:5:{{.*}}error:{{.*}}open array type cannot be used as variable type.
  CHECK: {{.*}}:15:34:{{.*}}error:{{.*}}open array type cannot be used as element type of non-open array type.
*)
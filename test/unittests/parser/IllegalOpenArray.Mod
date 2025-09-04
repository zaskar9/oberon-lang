(*
  RUN: not %oberon --run %s | filecheck %s
*)
MODULE IllegalOpenArray;

TYPE
  Object*     = POINTER TO ObjectDesc;
  ObjectDesc* = RECORD END;

  Storage* = POINTER TO ARRAY OF Object;

  Vector*     = POINTER TO VectorDesc;
  VectorDesc* = RECORD (ObjectDesc)
    storage: Storage;
    firstIdx, lastIdx: INTEGER;
  END;

END IllegalOpenArray.
(*
  CHECK: {{.*}}:10:25:{{.*}}error:{{.*}}undefined array dimension.
  CHECK: {{.*}}:10:14:{{.*}}error:{{.*}}pointer base type must be a record type.
*)
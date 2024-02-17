(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Boolean2;

IMPORT Out;

PROCEDURE Test;
  VAR t, f : BOOLEAN;
BEGIN
  t := TRUE;
  f := FALSE;
  IF t THEN
    IF t = TRUE THEN
      IF ~f THEN
        IF t # f THEN
          Out.String("PASS"); Out.Ln;
          RETURN
        END
      END
    END
  END;
  Out.String("FAIL"); Out.Ln
END Test;

BEGIN
    Test
END Boolean2.
(*
    CHECK: PASS
*)
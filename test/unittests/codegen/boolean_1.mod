(*
  RUN: %oberon -I "%S;%inc" -L "%S;%lib" -l oberon --run %s | filecheck %s
*)
MODULE Boolean1;

IMPORT Out;

PROCEDURE Test;
BEGIN
  IF TRUE THEN
    IF TRUE = TRUE THEN
      IF ~FALSE THEN
        IF TRUE # FALSE THEN
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
END Boolean1.
(*
    CHECK: PASS
*)
MODULE Oberon;

(* Import `time` function from C <time.h> library. *)
PROCEDURE time(VAR t: LONGINT): LONGINT; EXTERN;

PROCEDURE Time*(): LONGINT;
VAR t: LONGINT;
BEGIN
    time(t);
    RETURN t
END Time;

END Oberon.
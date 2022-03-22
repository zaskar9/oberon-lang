MODULE Oberon;

CONST TIME_UTC = 1;

TYPE TimeSpec = RECORD secs, nsecs : LONGINT END;

(* Import `printf` function from C <stdio.h> library. *)
PROCEDURE printf(format: STRING; ...): INTEGER; EXTERN;

(* Import `time` function from C <time.h> library. *)
PROCEDURE time(t: LONGINT): LONGINT; EXTERN;

(* Import `timespec_get` function from C <time.h> library. *)
PROCEDURE timespec_get(VAR ts: TimeSpec; base: INTEGER); EXTERN;

PROCEDURE Time*(): LONGINT;
VAR t: LONGINT;
BEGIN
    RETURN time(0)
END Time;

PROCEDURE TimeMillis*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    timespec_get(ts, TIME_UTC);
    printf("[%d,%d]", ts.secs, ts.nsecs);
    RETURN ts.secs * 1000 + ts.nsecs DIV 1000000
END TimeMillis;

END Oberon.
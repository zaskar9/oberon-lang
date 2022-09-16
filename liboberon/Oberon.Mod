MODULE Oberon;

CONST TIME_UTC* = 1;

TYPE TTime* = LONGINT;
     TimeSpec* = RECORD
                    secs*: TTime;
                    nsecs*: LONGINT
                END;

VAR ts*: TimeSpec;

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
    RETURN ts.secs * 1000 + ts.nsecs DIV 1000000
END TimeMillis;

PROCEDURE TimeMicros*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    timespec_get(ts, TIME_UTC);
    RETURN ts.secs * 1000000 + ts.nsecs DIV 1000
END TimeMicros;

PROCEDURE TimeNanos*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    timespec_get(ts, TIME_UTC);
    RETURN ts.secs * 1000000000 + ts.nsecs
END TimeNanos;

END Oberon.
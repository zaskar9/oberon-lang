MODULE Oberon;

CONST TIMEUTC* = 1;

TYPE TTime* = LONGINT;
     TimeSpec* = RECORD
                    secs*: TTime;
                    nsecs*: LONGINT
                END;

VAR ts*: TimeSpec;

(* Import `time` function from C <time.h> library. *)
PROCEDURE time(t: LONGINT): LONGINT; EXTERN;

(* Import `timespec_get` function from C <time.h> library. *)
PROCEDURE rt_timespec_get(VAR ts: TimeSpec; base: INTEGER); EXTERN;

PROCEDURE Time*(): LONGINT;
VAR t: LONGINT;
BEGIN
    RETURN time(0)
END Time;

PROCEDURE TimeMillis*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    rt_timespec_get(ts, TIMEUTC);
    RETURN ts.secs * 1000 + ts.nsecs DIV 1000000
END TimeMillis;

PROCEDURE TimeMicros*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    rt_timespec_get(ts, TIMEUTC);
    RETURN ts.secs * 1000000 + ts.nsecs DIV 1000
END TimeMicros;

PROCEDURE TimeNanos*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    rt_timespec_get(ts, TIMEUTC);
    RETURN ts.secs * 1000000000 + ts.nsecs
END TimeNanos;

END Oberon.
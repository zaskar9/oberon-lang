(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

(** Module with various system functions *)
MODULE Oberon;

CONST TIMEUTC* = 1;

TYPE TTime* = LONGINT;
     TimeSpec* = RECORD
                    secs*: TTime;
                    nsecs*: LONGINT
                END;

VAR ts*: TimeSpec;


(* Declare `long time(long)` from C <time.h> library. *)
PROCEDURE time(t: LONGINT): LONGINT; EXTERN;

(* Declare `void olang_oberon_timespec_get(struct timespec *, void *, int32_t * )` from Oberon runtime library. *)
PROCEDURE olang_oberon_timespec_get(VAR ts: TimeSpec; base: INTEGER); EXTERN;


(** Return elapsed clock ticks since program start *)
PROCEDURE Time*(): LONGINT;
VAR t: LONGINT;
BEGIN
    RETURN time(0)
END Time;

(** Return elapsed milliseconds since program start *)
PROCEDURE TimeMillis*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    olang_oberon_timespec_get(ts, TIMEUTC);
    RETURN ts.secs * 1000 + ts.nsecs DIV 1000000
END TimeMillis;

(** Return elapsed microseconds since program start *)
PROCEDURE TimeMicros*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    olang_oberon_timespec_get(ts, TIMEUTC);
    RETURN ts.secs * 1000000 + ts.nsecs DIV 1000
END TimeMicros;

(** Return elapsed nanoseconds since program start *)
PROCEDURE TimeNanos*(): LONGINT;
VAR ts: TimeSpec;
BEGIN
    olang_oberon_timespec_get(ts, TIMEUTC);
    RETURN ts.secs * 1000000000 + ts.nsecs
END TimeNanos;

END Oberon.
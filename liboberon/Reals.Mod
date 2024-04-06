(* System 3, Release 2.3. *)

MODULE Reals;

VAR
    DefaultFCR*: SET; // only used by System Trap

(* Import `int rt_reals_expo(float)` from Oberon runtime library. *)
PROCEDURE rt_reals_expo(x: REAL): INTEGER; EXTERN;

(* Import `int rt_reals_expoL(double)` from Oberon runtime library. *)
PROCEDURE rt_reals_expoL(x: LONGREAL): INTEGER; EXTERN;

(* Import `float rt_reals_ten(int)` from Oberon runtime library. *)
PROCEDURE rt_reals_ten(e: INTEGER): REAL; EXTERN;

(* Import `double rt_reals_tenL(int)` from Oberon runtime library. *)
PROCEDURE rt_reals_tenL(e: INTEGER): LONGREAL; EXTERN;

(* Import `void rt_reals_convert(float, int, char * )` from Oberon runtime library. *)
PROCEDURE rt_reals_convert(x: REAL; n: INTEGER; VAR d: ARRAY OF CHAR): REAL; EXTERN;

(* Returns the shifted binary exponent (0 <= e < 256). *)
PROCEDURE Expo*(x: REAL): INTEGER;
BEGIN
    RETURN rt_reals_expo(x);
END Expo;

(* Returns the shifted binary exponent (0 <= e < 2048). *)
PROCEDURE ExpoL*(x: LONGREAL): INTEGER;
    VAR i: LONGINT;
BEGIN
    RETURN rt_reals_expoL(x);
END ExpoL;

(* Sets the shifted binary exponent. *)
PROCEDURE SetExpoL*(e: LONGINT; VAR x: LONGREAL);
    VAR i: LONGINT;
BEGIN
    (* TODO *)
END SetExpoL;

(* Convert hexadecimal to REAL. *)
PROCEDURE Real*(h: LONGINT): REAL;
    VAR x: REAL;
BEGIN
    RETURN 0 (* TODO *)
END Real;

(* Convert hexadecimal to LONGREAL. h and l are the high and low parts.*)
PROCEDURE RealL*(h, l: LONGINT): LONGREAL;
    VAR x: LONGREAL;
BEGIN
    RETURN 0 (* TODO *)
END RealL;

(* Convert REAL to hexadecimal. *)
PROCEDURE Int*(x: REAL): LONGINT;
    VAR i: LONGINT;
BEGIN
    RETURN 0 (* TODO *)
END Int;

(* Convert LONGREAL to hexadecimal. h and l are the high and low parts. *)
PROCEDURE IntL*(x: LONGREAL; VAR h, l: LONGINT);
BEGIN
    (* TODO *)
END IntL;

(* Returns 10^e (e <= 308, 308 < e delivers IEEE-code +INF). *)
PROCEDURE Ten*(e: INTEGER): REAL;
BEGIN
    RETURN rt_reals_ten(e);
END Ten;

PROCEDURE TenL*(e: INTEGER): LONGREAL;
BEGIN
    RETURN rt_reals_tenL(e);
END TenL;

PROCEDURE Convert*(x: REAL; n: INTEGER; VAR d: ARRAY OF CHAR);
BEGIN
    rt_reals_convert(x, n, d);
END Convert;

PROCEDURE ConvertL*(x: LONGREAL; n: INTEGER; VAR d: ARRAY OF CHAR);
BEGIN
    (* TODO *)
END ConvertL;

PROCEDURE ConvertH*(x: REAL; VAR d: ARRAY OF CHAR);
BEGIN
    (* TODO *)
END ConvertH;

PROCEDURE ConvertHL*(x: LONGREAL; VAR d: ARRAY OF CHAR);
BEGIN
    (* TODO *)
END ConvertHL;


(* Returns the NaN code (0 <= c < 8399608) or -1 if not NaN/Infinite. *)
PROCEDURE NaNCode*(x: REAL): LONGINT;
BEGIN
    RETURN 0 (* TODO *)
END NaNCode;

(* Returns the NaN code (0 <= h < 1048576, MIN(LONGINT) <= l <= MAX(LONGINT)) or (-1,-1) if not NaN/Infinite. *)
PROCEDURE NaNCodeL*(x: LONGREAL; VAR h, l: LONGINT);
BEGIN
    (* TODO *)
END NaNCodeL;

(* Returns NaN with specified code (0 <= h < 1048576, MIN(LONGINT) <= l <= MAX(LONGINT)). *)
PROCEDURE NaNL*(h, l: LONGINT): LONGREAL;
VAR x: LONGREAL;
BEGIN
    RETURN 0 (* TODO *)
END NaNL;

(* Return state of the floating-point control register. *)
(*PROCEDURE FCR*(): SET;
BEGIN
    RETURN DEFAULT(SET) (* TODO *)
END FCR;*)

(* Set state of floating-point control register.  Traps reset this to the default & ENTIER resets the rounding mode. *)
PROCEDURE SetFCR*(s: SET);
BEGIN
    (* TODO *)
END SetFCR;

END Reals.
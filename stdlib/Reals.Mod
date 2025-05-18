(* The Oberon modules of the standard library contained in this directory are based on ETH Oberon.
 * The original source code is used and distributed under the ETH Oberon License, see LICENSE file
 * for details. The modules have been adapted from the original source code to work with the LLVM
 * Oberon compiler and leverage its capabilities in interfacing with the host operating system.
 *)

MODULE Reals;

  VAR
    DefaultFCR*: SET; (* only used by System Trap. *)


  (* Import `int32_t olang_reals_expo(float)` from Oberon runtime library. *)
  PROCEDURE olang_reals_expo(x: REAL): INTEGER; EXTERN;

  (* Import `int32_t olang_reals_expoL(double)` from Oberon runtime library. *)
  PROCEDURE olang_reals_expoL(x: LONGREAL): INTEGER; EXTERN;

  (* Import `float olang_reals_ten(int32_t)` from Oberon runtime library. *)
  PROCEDURE olang_reals_ten(e: INTEGER): REAL; EXTERN;

  (* Import `double olang_reals_tenL(int32_t)` from Oberon runtime library. *)
  PROCEDURE olang_reals_tenL(e: INTEGER): LONGREAL; EXTERN;

  (* Import `float olang_reals_nan(int32_t)` from Oberon runtime library. *)
  PROCEDURE olang_reals_nan(): REAL; EXTERN;

  (* Import `int32_t olang_reals_nan_code(float)` from Oberon runtime library. *)
  PROCEDURE olang_reals_nan_code(x: REAL): INTEGER; EXTERN;

  (* Import `void olang_reals_nan_codeL(double, int32_t *, int32_t * )` from Oberon runtime library. *)
  PROCEDURE olang_reals_nan_codeL(x: LONGREAL; VAR h, l: LONGINT); EXTERN;

  (* Import `double olang_reals_nan()` from Oberon runtime library. *)
  PROCEDURE olang_reals_nanL(): LONGREAL; EXTERN;


  (* Returns the shifted binary exponent (0 <= e < 256). *)
  PROCEDURE Expo*(x: REAL): INTEGER;
  BEGIN
    RETURN olang_reals_expo(x)
  END Expo;

  (* Returns the shifted binary exponent (0 <= e < 2048). *)
  PROCEDURE ExpoL*(x: LONGREAL): INTEGER;
    VAR i: LONGINT;
  BEGIN
    RETURN olang_reals_expoL(x)
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
    RETURN olang_reals_ten(e)
  END Ten;

  PROCEDURE TenL*(e: INTEGER): LONGREAL;
  BEGIN
    RETURN olang_reals_tenL(e)
  END TenL;

  PROCEDURE ConvertL*(x: LONGREAL; n: INTEGER; VAR d: ARRAY OF CHAR);
    VAR i, j, k: LONGINT;
  BEGIN
    IF x < 0 THEN x := -x END;
    k := 0;
    IF (SIZE(LONGINT) < 8) & (n > 9) THEN
      (* there are more decimal digits than can be held in a single LONGINT *)
      i := ENTIER(x /      1000000000.0E0);  (* higher digits *)
      j := ENTIER(x - (i * 1000000000.0E0)); (* lower digits *)
      (* first generate the lower 9 digits. *)
      IF j < 0 THEN j := 0 END;
      WHILE k < 9 DO
        d[k] := CHR(SHORT(j MOD 10) + 30H); j := j DIV 10; INC(k)
      END
      (* fall through to generate the higher digits *)
    ELSE
      i := ENTIER(x)
    END;
    WHILE k < n DO
      d[k] := CHR(SHORT(i MOD 10) + 48); i := i DIV 10; INC(k)
    END
  END ConvertL;

  PROCEDURE Convert*(x: REAL; n: INTEGER; VAR d: ARRAY OF CHAR);
  BEGIN
    ConvertL(x, n, d)
  END Convert;

  PROCEDURE ConvertH*(x: REAL; VAR d: ARRAY OF CHAR);
  BEGIN
    (* TODO *)
  END ConvertH;

  PROCEDURE ConvertHL*(x: LONGREAL; VAR d: ARRAY OF CHAR);
  BEGIN
    (* TODO *)
  END ConvertHL;

  (* Returns NaN with the specified code (0 <= c < 8399608). *)
  PROCEDURE NaN*(): REAL;
  BEGIN
    RETURN olang_reals_nan()
  END NaN;

  (* Returns the NaN code (0 <= c < 8399608) or -1 if not NaN/Infinite. *)
  PROCEDURE NaNCode*(x: REAL): LONGINT;
  BEGIN
    RETURN olang_reals_nan_code(x)
  END NaNCode;

  (* Returns the NaN code (0 <= h < 1048576, MIN(LONGINT) <= l <= MAX(LONGINT)) or (-1,-1) if not NaN/Infinite. *)
  PROCEDURE NaNCodeL*(x: LONGREAL; VAR h, l: LONGINT);
  BEGIN
    h := 0; l := 0; (* zero out all 64 bits *)
    olang_reals_nan_codeL(x, h, l)
  END NaNCodeL;

  (* Returns NaN with the specified code (0 <= c < 8399608). *)
  PROCEDURE NaNL*(): LONGREAL;
  BEGIN
    RETURN olang_reals_nanL()
  END NaNL;

  (* Return state of the floating-point control register. *)
  (*PROCEDURE FCR*(): SET;
  BEGIN
    (* TODO *)
    RETURN DEFAULT(SET)
  END FCR;*)

  (* Set state of floating-point control register.  Traps reset this to the default & ENTIER resets the rounding mode. *)
  PROCEDURE SetFCR*(s: SET);
  BEGIN
    (* TODO *)
  END SetFCR;
  

END Reals.
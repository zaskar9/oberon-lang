MODULE Math;

CONST e* = 2.7182818284;
      pi* = 3.1415926535;

PROCEDURE sqrtf(x: REAL): REAL; EXTERN;
PROCEDURE expf(x: REAL): REAL; EXTERN;
PROCEDURE logf(x: REAL): REAL; EXTERN;
PROCEDURE sinf(x: REAL): REAL; EXTERN;
PROCEDURE cosf(x: REAL): REAL; EXTERN;
PROCEDURE atanf(x: REAL): REAL; EXTERN;
PROCEDURE rt_realf(x: INTEGER): REAL; EXTERN;
PROCEDURE rt_entierf(x: REAL): INTEGER; EXTERN;

PROCEDURE sqrt*(x: REAL): REAL;
BEGIN
    RETURN sqrtf(x)
END sqrt;

PROCEDURE exp*(x: REAL): REAL;
BEGIN
    RETURN expf(x)
END exp;

PROCEDURE ln*(x: REAL): REAL;
BEGIN
    RETURN logf(x)
END ln;

PROCEDURE sin*(x: REAL): REAL;
BEGIN
    RETURN sinf(x)
END sin;

PROCEDURE cos*(x: REAL): REAL;
BEGIN
    RETURN cosf(x)
END cos;

PROCEDURE arctan*(x: REAL): REAL;
BEGIN
    RETURN atanf(x)
END arctan;

PROCEDURE real*(x: INTEGER): REAL;
BEGIN
    RETURN rt_realf(x)
END real;

PROCEDURE entier*(x: REAL): INTEGER;
BEGIN
    RETURN rt_entierf(x)
END entier;

END Math.


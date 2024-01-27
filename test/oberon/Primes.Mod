MODULE Primes;
IMPORT Out;

CONST MAX = 1000;

VAR n: INTEGER;

(* Lists all prime numbers up to `max` using the Sieve of Eratosthenes algorithm. *)
PROCEDURE sieve(max: INTEGER);
VAR del: ARRAY MAX OF BOOLEAN;
    n, i, j: INTEGER;
BEGIN
    IF max > MAX - 1 THEN n := MAX - 1 ELSE n := max END;
    FOR i := 0 TO n DO del[i] := FALSE END;
    i := 2;
    REPEAT
      IF ~del[i] THEN
        j := i * i;
        WHILE j <= n DO
          del[j] := TRUE;
          j := j + i
        END
      END;
      i := i + 1
    UNTIL i * i > n;
    FOR i := 2 TO n DO
        IF ~del[i] THEN Out.Int(i, 0); Out.String(" ") END
    END
END sieve;

(* Checks whether a given number is a prime number. *)
PROCEDURE isPrime(n: INTEGER): BOOLEAN;
VAR m: INTEGER;
    found: BOOLEAN;
BEGIN
    m := 2;
    found := FALSE;
    WHILE (m <= n-1) & (~found) DO
        IF (n MOD m) = 0 THEN
            found := TRUE
        ELSE
            m := m + 1
        END
    END;
    RETURN ~found
END isPrime;

BEGIN
    ASSERT(~isPrime(16));
    n := 17;
    Out.String("The number "); Out.Int(n, 0); Out.String(" is ");
    IF ~isPrime(n) THEN
        Out.String("not ")
    END;
    Out.String("a prime number."); Out.Ln;
    ASSERT(isPrime(17));
    sieve(200); Out.Ln
END Primes.

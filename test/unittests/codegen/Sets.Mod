(*
  RUN: %oberon -I "%S%{pathsep}%inc" -L "%S%{pathsep}%lib" -l oberon --run %s | filecheck %s
*)
MODULE Sets; (* Tests for datatype SET, based on https://people.inf.ethz.ch/wirth/Oberon/SETs.pdf. *)
IMPORT Out;

CONST
    EmptySet = {};
    FullSet = { 0 .. 31 };

VAR s: SET;
    v: INTEGER;

    PROCEDURE P1;
    VAR s0, s1, s2: SET;
        m, n: INTEGER;
    BEGIN
        m := 1; n := 3;
        s1 := { m .. n }; s2 := { m + 2 .. n + 2 };
        s0 := s1 + s2;
        ASSERT(s0 = { 1 .. 5 });
        s0 := s1 * s2;
        ASSERT(s0 = { 3 });
        s0 := s1 - s2;
        ASSERT(s0 = { 1, 2 });
        s0 := s1 / s2;
        ASSERT(s0 = { 1, 2, 4, 5 });
        s0 := -s1;
        ASSERT(s0 = { 0, 4 .. 31 });
        s0 := s1 * s2 + s1 / s2;
        ASSERT(s0 = { 1 .. 5 })
    END P1;

    PROCEDURE P2;
    VAR s0, s1, s2: SET;
    BEGIN
        s0 := {}; s0 := { 0 }; s0 := { 8, 10 .. 12, 15 }; s1 := FullSet;
        s2 := { 0, 1 } + { 0, 2 }; (* set union *)
        s2 := { 0, 1 } - { 0, 2 }; (* set difference *)
        s2 := { 0, 1 } * { 0, 2 }; (* set intersection *)
        s2 := { 0, 1 } / { 0, 2 }; (* set symmetric difference *)
        s2 := -FullSet;
        s1 := { 0, 1 };
        s2 := s2;
        IF s1 = s2 THEN s2 := { 0, 2 } END;
        IF s1 # s2 THEN s2 := { 0, 1, 2, 3 } END;
        ASSERT(1 IN s2);
        s1 := { 10 .. 20 };
        ASSERT(s1 <= FullSet);
        s1 := FullSet; s2 := EmptySet;
        ASSERT(s1 >= s2)
    END P2;

    PROCEDURE P3;
    VAR s0, s1, s2: SET;
        i, j, m, n: INTEGER;
    BEGIN
        m := 7; n := 15;
        s0 := { 1 .. 5 };
        ASSERT(s0 = { 1, 2, 3, 4, 5 });
        i := 5; j := 10;
        s0 := { 0, i MOD 2, i .. j, 13, i + 10 .. j + 10, 25 .. 30, 31 };
        ASSERT(s0 = { 0, 1, 5, 6, 7, 8, 9, 10, 13, 15, 16, 17, 18, 19, 20, 25, 26, 27, 28, 29, 30, 31 });
        s0 := { m };
        ASSERT(s0 = { 7 });
        s0 := { m, n };
        ASSERT(s0 = { 7, 15 });
        s0 := { m .. n };
        ASSERT(s0 = { 7, 8, 9, 10, 11, 12, 13, 14, 15 });
        s0 := { 1 .. 3, m .. n };
        ASSERT(s0 = { 1, 2, 3, 7, 8, 9, 10, 11, 12, 13, 14, 15 });
        s0 := { n - m .. n + m };
        ASSERT(s0 = { 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 });
        ASSERT(n IN s0);
        s1 := { m + 2 .. n - 2}; s2 := { m .. n };
        ASSERT(s1 <= s2);
        ASSERT(s2 >= s1)
    END P3;

BEGIN
    Out.Set(FullSet); Out.Ln;
    Out.Set(EmptySet); Out.Ln;
    P1;
    P2;
    P3;
    s := EmptySet; v := 12;
    INCL(s, v);
    ASSERT(v IN s);
    EXCL(s, v);
    ASSERT(~(v IN s));
    ASSERT(s = EmptySet);
    Out.Hex(ORD(FullSet)); Out.Ln
END Sets.
(*
  CHECK: { 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 }
  CHECK: { }
  CHECK: FFFFFFFF
*)
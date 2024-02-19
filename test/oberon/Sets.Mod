(* Tests for datatype SET, based on https://people.inf.ethz.ch/wirth/Oberon/SETs.pdf. *)
MODULE Sets;
IMPORT Out;

CONST
    EmptySet = {};
    FullSet = { 0 .. 31 };

VAR s: SET;
    v: INTEGER;

    PROCEDURE P1(m, n: INTEGER);
    VAR s0, s1, s2: SET;
    BEGIN
        s1 := { m .. n }; s2 := { m + 2 .. n + 2 };
        s0 := s1 + s2; Out.Set(s0); Out.Ln;
        s0 := s1 * s2; Out.Set(s0); Out.Ln;
        s0 := s1 - s2; Out.Set(s0); Out.Ln;
        s0 := s1 / s2; Out.Set(s0); Out.Ln;
        s0 := -s1; Out.Set(s0); Out.Ln;
        s0 := s1 * s2 + s1 / s2; Out.Set(s0); Out.Ln
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
        IF { 0, 1 } = { 0, 1 } THEN s2 := EmptySet END;
        IF { 0, 1 } # { 0, 2 } THEN s2 := EmptySet END;
        IF 1 IN { 0, 1, 2, 3 } THEN s2 := EmptySet END;
        IF { 10 .. 20 } <= FullSet THEN s2 := EmptySet END;
        IF FullSet >= EmptySet THEN s2 := EmptySet END
    END P2;

    PROCEDURE P3(m, n: INTEGER);
    VAR s0, s1, s2: SET;
        i, j: INTEGER;
    BEGIN
        s0 := { 1 .. 5 }; Out.Set(s0); Out.Ln;
        i := 5; j := 10;
        s0 := { 0, i MOD 2, i .. j, 13, i + 10 .. j + 10, 25 .. 30, 31 }; Out.Set(s0); Out.Ln;
        s0 := { m }; Out.Set(s0); Out.Ln;
        s0 := { m, n }; Out.Set(s0); Out.Ln;
        s0 := { m .. n }; Out.Set(s0); Out.Ln;
        s0 := { 1 .. 3, m .. n }; Out.Set(s0); Out.Ln;
        s0 := { n - m .. n + m }; Out.Set(s0); Out.Ln;
        ASSERT(n IN s0);
        s1 := { m + 2 .. n - 2}; s2 := { m .. n };
        ASSERT(s1 <= s2);
        ASSERT(s2 >= s1)
    END P3;

BEGIN
    Out.Set(FullSet); Out.Ln;
    Out.Set(EmptySet); Out.Ln;
    P1(1, 3); Out.Ln;
    P3(7, 15);
    s := EmptySet; v := 12;
    INCL(s, v); Out.Set(s); Out.Ln; ASSERT(v IN s);
    EXCL(s, v); Out.Set(s); Out.Ln; ASSERT(~(v IN s))
END Sets.
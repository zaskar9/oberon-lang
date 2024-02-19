(* Tests for datatype SET, based on https://people.inf.ethz.ch/wirth/Oberon/SETs.pdf. *)
MODULE Sets;
IMPORT Out;

CONST
    EmptySet = {};
    FullSet = { 0 .. 31 };

VAR
    set: SET;

    PROCEDURE OutSet(set: SET);
    VAR i: INTEGER;
    BEGIN
        Out.String("{ ");
        FOR i := 0 TO 31 DO
            IF i IN set THEN
                Out.Int(i, 0); Out.String(" ")
            END
        END;
        Out.String("}")
    END OutSet;

(*
    PROCEDURE P1;
    VAR s0, s1, s2: SET;
    BEGIN
        s0 := s1 + s2; s0 := s1 * s2; s0 := s1 - s2; s0 := s1 / s2;
        s0 := -s1; s0 := s0 * s1 + s0 / s1
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
*)

    PROCEDURE P3(m, n: INTEGER);
    VAR s0, s1, s2: SET;
    BEGIN
        s0 := { m }; OutSet(s0); Out.Ln;
        s0 := { m, n }; OutSet(s0); Out.Ln;
        s0 := { m .. n }; OutSet(s0); Out.Ln;
        s0 := { 1 .. 3, m .. n }; OutSet(s0); Out.Ln;
        s0 := { n + m .. n - m }; OutSet(s0); Out.Ln
        (* IF n IN s0 THEN s0 := { 0 } END; *)
        (* IF s1 <= s2 THEN s0 := { 4 } END *)
    END P3;

BEGIN
    set := FullSet;
    OutSet(set); Out.Ln;
    P3(7, 15)
END Sets.
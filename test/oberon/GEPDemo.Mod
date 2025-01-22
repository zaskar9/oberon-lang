MODULE GEPDemo;
IMPORT Out;

CONST
    One = 1;
    Two = 2;
    Three = 3;

TYPE
    Vector = ARRAY 20 OF INTEGER;
    Tuple = RECORD fst, snd: INTEGER END;
    Triple = RECORD tuple: Tuple; trd: INTEGER END;
    TripleList = ARRAY 10 OF Triple;

VAR
    triples: TripleList;
    vec: Vector;

BEGIN
    triples[9].tuple.fst := One;
    triples[9].tuple.snd := Two;
    triples[9].trd := Three;
    ASSERT(triples[9].tuple.fst = One);
    ASSERT(triples[9].tuple.snd = Two);
    ASSERT(triples[9].trd = Three);
    Out.String("All assertions passed!"); Out.Ln
END GEPDemo.

MODULE Inference;

VAR l: LONGINT;

BEGIN
    l := MIN(INTEGER);
    ASSERT(ROR(SHORT(l), 31) = 1)
END Inference.
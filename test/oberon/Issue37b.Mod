MODULE Issue37b;
IMPORT Out;

PROCEDURE IsLower (ch: CHAR) : BOOLEAN;
BEGIN RETURN (ch >= "a") & (ch <= "z")
END IsLower;

PROCEDURE Upper(ch: CHAR) : CHAR;
BEGIN
    IF IsLower(ch) THEN RETURN CHR(ORD(ch) + (ORD("A") - ORD("a"))) END;
    RETURN ch
END Upper;

PROCEDURE Length(str: ARRAY OF CHAR) : INTEGER;
VAR i: INTEGER;
BEGIN
    i := 0;
    WHILE (i < LEN(str)) & (str[i] # 00X) DO INC(i) END;
    RETURN i
END Length;

PROCEDURE IMatch1(str, pattern : ARRAY OF CHAR; IgnoreCase : BOOLEAN; is, ip: LONGINT): BOOLEAN;
VAR
    lens, lenp : LONGINT;
BEGIN
    lens := Length(str);
    lenp := Length(pattern);
    WHILE ip < lenp DO
        IF pattern[ip] = "*" THEN
            WHILE is <= lens DO (* check to end of string *)
                IF IMatch1(str, pattern, IgnoreCase, is, ip + 1) THEN RETURN TRUE END;
                INC(is)
            END;
            RETURN FALSE
        ELSIF is = lens THEN (* pattern not exhausted *)
            RETURN FALSE
        ELSIF pattern[ip] = "?" THEN
        ELSIF ~IgnoreCase & (str[is] # pattern[ip]) THEN
            RETURN FALSE
        ELSIF IgnoreCase & (Upper(str[is]) # Upper(pattern[ip])) THEN
            RETURN FALSE
        END;
        INC(is); INC(ip)
    END;
    RETURN is = lens
END IMatch1;

PROCEDURE Match1 (str, pattern : ARRAY OF CHAR; IgnoreCase : BOOLEAN): BOOLEAN;
BEGIN
    IF Length(pattern) = 0 THEN RETURN Length(str) = 0 END;
    RETURN IMatch1(str, pattern, IgnoreCase, 0, 0)
END Match1;

PROCEDURE Match2 (str, pattern : ARRAY OF CHAR; IgnoreCase : BOOLEAN): BOOLEAN;
    PROCEDURE IMatch(str, pattern : ARRAY OF CHAR; IgnoreCase : BOOLEAN; is, ip: LONGINT): BOOLEAN;
    VAR
        lens, lenp : LONGINT;
    BEGIN
        lens := Length(str);
        lenp := Length(pattern);
        WHILE ip < lenp DO
            IF pattern[ip] = "*" THEN
                WHILE is <= lens DO (* check to end of string *)
                    IF IMatch(str, pattern, IgnoreCase, is, ip + 1) THEN RETURN TRUE END;
                    INC(is)
                END;
                RETURN FALSE
            ELSIF is = lens THEN (* pattern not exhausted *)
                RETURN FALSE
            ELSIF pattern[ip] = "?" THEN
            ELSIF ~IgnoreCase & (str[is] # pattern[ip]) THEN
                RETURN FALSE
            ELSIF IgnoreCase & (Upper(str[is]) # Upper(pattern[ip])) THEN
                RETURN FALSE
            END;
            INC(is); INC(ip)
        END;
        RETURN is = lens
    END IMatch;
BEGIN
    IF Length(pattern) = 0 THEN RETURN Length(str) = 0 END;
    RETURN IMatch(str, pattern, IgnoreCase, 0, 0)
END Match2;

BEGIN
    Out.String("Match1('Filename.exe', 'f?l*.exe', FALSE) = "); Out.Int(ORD(Match1("Filename.exe", "f?l*.exe", FALSE)), 0); Out.Ln;
    Out.String("Match2('Filename.exe', 'f?l*.exe', FALSE) = "); Out.Int(ORD(Match2("Filename.exe", "f?l*.exe", FALSE)), 0); Out.Ln
END Issue37b.
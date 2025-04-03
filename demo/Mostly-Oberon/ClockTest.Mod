(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
 MODULE ClockTest;

IMPORT Tests, Chars, Clock; (* , Out; *)

CONST
    MAXSTR = Chars.MAXSTR;

VAR
    title : ARRAY MAXSTR OF CHAR;
    success, errors : INTEGER;

PROCEDURE TestGetRtcTime() : BOOLEAN;
VAR second, minute, hour, day, month, year : INTEGER; test, expected, result: BOOLEAN;
BEGIN
    test := TRUE;
    second := 0; minute := 0; hour := 0;
    day := 0; month := 0; year := 0;
    expected := TRUE;
    Clock.GetRtcTime(second, minute, hour, day, month, year);


    result := (year > 1900);
    Tests.ExpectedBool(expected, result, "year should be greater than 1900", test);
    result := (month >= 0) & (month <= 11);
    Tests.ExpectedBool(expected, result, "month should be [0, 11]", test);
    result := (day >= 1) & (day <= 31);
    Tests.ExpectedBool(expected, result, "day should be non-zero", test);

    result := (hour >= 0) & (hour <= 23);
    Tests.ExpectedBool(expected, result, "hour should be [0, 23]", test);
    result := (minute >= 0) & (minute <= 59);
    Tests.ExpectedBool(expected, result, "minute should be [0, 59]", test);
    result := (second >= 0) & (second <= 60);
    Tests.ExpectedBool(expected, result, "second year should be [0,60]", test);
    RETURN test
END TestGetRtcTime;

PROCEDURE TestGet() : BOOLEAN;
VAR time, date : INTEGER; test, expected, result : BOOLEAN;
BEGIN
    test := TRUE;
    time := 0;
    date := 0;
    Clock.Get(time, date);
    expected := TRUE;
    result := (time > 0);
    Tests.ExpectedBool(expected, result, "time should not be zero", test);
    result := (date > 0);
    Tests.ExpectedBool(expected, result, "date should not be zero", test);

    RETURN test
END TestGet;

BEGIN
    Chars.Set("Clock module test", title);
    success := 0; errors := 0;
    Tests.Test(TestGetRtcTime, success, errors);
    Tests.Test(TestGet, success, errors);
    Tests.Summarize(title, success, errors);
END ClockTest.

ClockTest
=========

Test the clock module providing real time clock access
via Clock.Mod wrapping C level functions.


(* DatesTest.Mod - test the Dates module.

Copyright (C) 2020 R. S. Doiel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.


@Author R. S. Doiel, <rsdoiel@gmail.com>
copyright (c) 2020, all rights reserved.
This software is released under the GNU AGPL
See http://www.gnu.org/licenses/agpl-3.0.html
*)
MODULE DatesTest;

IMPORT Tests, Chars, Dates;

CONST
    MAXSTR = Chars.MAXSTR;

VAR
    title : ARRAY MAXSTR OF CHAR;
    success, errors : INTEGER;

PROCEDURE TestIsDateString() : BOOLEAN;
VAR 
    test, result, expected : BOOLEAN;
    text, msg : ARRAY MAXSTR OF CHAR;
    dt : Dates.DateTime;
BEGIN
    test := TRUE; (* Assume a successful test *)
    (* From command line arg '2020-10-26' *)
    Chars.Set("2020-10-26", text);
    expected := TRUE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg);
    Chars.AppendChars(" is valid parse -> ", msg);
    Dates.ToChars(dt, Dates.YYYYMMDD, text);
    Chars.AppendChars(text, msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* From command line arg '2020-11-20' *)
    Chars.Set("2020-11-20", text);
    expected := TRUE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg);
    Chars.AppendChars(" is valid parse", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* Simple text expected the dates in YYYY-MM-DD *)
    Chars.Set("2015-07-04", text);
    expected := TRUE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* Simple text expected the dates in MM/DD/YYYY *)
    Chars.Set("07/04/2015", text);
    expected := TRUE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* IsDateString expects dates in YYYYY-MM-DD format. *)
    Chars.Set("January 20th, 2015", text);
    expected := FALSE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid, IsDateString()", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* Also not valid for IsDateString... *)
    Chars.Set("07-25-2015", text);
    expected := FALSE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* This is an entry not a DateString *)
    Chars.Set("08:00 - 9:00; misc; email and what not.", text);
    expected := FALSE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* This is an empty line, not a DateString *)
    Chars.Clear(text);
    expected := FALSE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* This is just some random text, not a Date String *)
    Chars.Set("This is just some random text, not a Date String", text);
    expected := FALSE;
    result := Dates.IsDateString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    RETURN test
END TestIsDateString;

PROCEDURE TestIsTimeString() : BOOLEAN;
VAR 
    test, result, expected : BOOLEAN;
    text, msg : ARRAY MAXSTR OF CHAR;
BEGIN
    test := TRUE; (* Assume a successful test *)
    (* Simple text expected the time in HH:MM:SS, 
       HH:MM, H:MM:SS, H:MM formats where an additional
       'a', 'am', 'p' or 'pm' will indicate a 12 or 24 hour
       form. *)
    Chars.Set("01:22:22", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("13:04", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03p", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03pm", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03a", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03am", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03P", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03PM", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03A", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03AM", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03 am", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03 AM", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03 pm", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("6:03 pm", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* Also not valid for IsTimeString ... *)
    Chars.Set("6:03:02", text);
    expected := FALSE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is a time string", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* NOTE: IsTimeString does not parse the string so does not
    check the values of numbers in the HH:MM:SS sequence *)
    Chars.Set("50:33:32", text);
    expected := TRUE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("12:133:32", text);
    expected := FALSE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("12:13z", text);
    expected := FALSE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Clear(text);
    expected := FALSE;
    result := Dates.IsTimeString(text);
    Chars.Set("''", msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    Chars.Set("This is just some random text, not a Date String", text);
    expected := FALSE;
    result := Dates.IsTimeString(text);
    Chars.Set(text, msg);
    Chars.AppendChars(" is NOT valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    RETURN test
END TestIsTimeString;

PROCEDURE TestIsValidDate() : BOOLEAN;
VAR test, expected, result : BOOLEAN;  dt : Dates.DateTime; msg, ymd : ARRAY MAXSTR OF CHAR;
BEGIN
    test := TRUE;
    dt.year := 2020;
    dt.month := 11;
    dt.day := 16;
    expected := TRUE;
    result := Dates.IsValidDate(dt);
    Dates.ToChars(dt, Dates.YYYYMMDD, ymd);
    Chars.Set("DateIsValidDate(dt) should be TRUE for ", msg);
    Chars.AppendChars(ymd, msg);
    Tests.ExpectedBool(expected, result, msg, test);

    expected := FALSE;
    dt.year := 0;
    dt.month := 0;
    dt.day := 1;
    result := Dates.IsValidDate(dt);
    Dates.ToChars(dt, Dates.YYYYMMDD, ymd);
    Chars.Set("DateIsValidDate(dt) should be FALSE for ", msg);
    Chars.AppendChars(ymd, msg);
    Tests.ExpectedBool(expected, result, msg, test);

    RETURN test
END TestIsValidDate;

PROCEDURE TestParse() : BOOLEAN;
VAR 
    test, expected, result : BOOLEAN; 
    text, msg, s1, s2 : ARRAY MAXSTR OF CHAR;
    days, hours, minutes, seconds : INTEGER;
    dt, edt : Dates.DateTime;
BEGIN
    (* NOTE: I'm setting a default date *)
    Dates.SetDate(2020, 11, 16, dt);
    test := TRUE; (* Assume a successful test *)
    (* Simple text expected the dates in YYYY-MM-DD *)
    Chars.Set("2015-07-04", text);
    expected := TRUE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg); Chars.AppendChars(" is valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* Simple text expected the dates in MM/DD/YYYY transformed
       into YYYY-MM-DD *)
    Dates.SetDate(2020, 11, 16, dt);
    Dates.SetDate(2015, 7, 4, edt);
    Chars.Set("07/04/2015", text);
    expected := TRUE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg); Chars.AppendChars(" is valid", msg);
    Tests.ExpectedBool(expected, result, msg, test);
    expected := TRUE;
    Chars.Set("CompareDate() should match, ", msg);
    Dates.ToChars(edt, Dates.YYYYMMDD, s1);
    Dates.ToChars(dt, Dates.YYYYMMDD, s2);
    Chars.AppendChars(s1, msg);
    Chars.AppendChars(" ? ", msg);
    Chars.AppendChars(s2, msg);
    result := (Dates.CompareDate(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, msg, test);

    Dates.SetDate(2020, 11, 16, dt);
    Dates.SetDate(2015, 7, 4, edt);
    expected := FALSE;
    result := (Dates.CompareDate(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "CompareDate NOT should match", test);
    Dates.Set(2020, 11, 16, 12, 12, 12, dt);
    Dates.Set(2020, 11, 16, 12, 12, 12, edt);
    expected := TRUE;
    result := (Dates.CompareTime(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "CompareTime should match", test);
    result := (Dates.CompareDate(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "CompareDate should match", test);
    result := (Dates.Compare(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "Compare should match", test);

    Dates.Set(2015, 11, 16, 1, 12, 12, dt);
    Dates.Set(2020, 11, 16, 12, 12, 12, edt);
    expected := FALSE;
    result := (Dates.CompareTime(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "CompareTime NOT should match", test);
    result := (Dates.CompareDate(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "CompareDate NOT should match", test);
    result := (Dates.Compare(edt, dt) = 0);
    Tests.ExpectedBool(expected, result, "Compare NOT should match", test);

    Dates.Set(2020, 11, 16, 12, 12, 12, dt);
    Dates.Set(2020, 11, 16, 12, 12, 12, edt);
    expected := TRUE;
    Dates.TimeDifference(edt, dt, days, hours, minutes, seconds) ;
    expected := TRUE;
    result := (days = 0);
    Chars.Set("TimeDifference should be zero days found ", msg);
    Chars.AppendInt(days, 1, " ", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    result := (hours = 0);
    Chars.Set("TimeDifference should be zero hours found ", msg);
    Chars.AppendInt(hours, 1, " ", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    result := (minutes = 0);
    Chars.Set("TimeDifference should be zero minutes found ", msg);
    Chars.AppendInt(minutes, 1, " ", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    result := (seconds = 0);
    Chars.Set("TimeDifference should be zero seconds found ", msg);
    Chars.AppendInt(seconds, 1, " ", msg);
    Tests.ExpectedBool(expected, result, msg, test);



    (* ParseDate expects dates in YYYYY-MM-DD format. *)
    Chars.Set("January 20th, 2015", text);
    expected := FALSE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg); Chars.AppendChars(" is NOT valid, Parse(text, dt)", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* Also not valid for date, i.e. dashes instead of slashes ... *)
    Chars.Set("07-25-2015", text);
    expected := FALSE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg); Chars.AppendChars(" is NOT valid, Parse(text, dt)", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* This is an empty line, not a Date *)
    Chars.Clear(text);
    expected := FALSE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg); Chars.AppendChars(" is NOT valid, Parse(text, dt)", msg);
    Tests.ExpectedBool(expected, result, msg, test);

    (* This is just some random text, not a Date*)
    Chars.Set("This is just some random text, not a DateTime string.", text);
    expected := FALSE;
    result := Dates.Parse(text, dt);
    Chars.Set(text, msg); Chars.AppendChars(" is NOT valid, Parse(text, dt)", msg);
    Tests.ExpectedBool(expected, result, msg, test);
    RETURN test
END TestParse;

BEGIN
    Chars.Clear(title);
    title := "Date module tests";
    success := 0;
    errors := 0;
    Tests.Test(TestIsTimeString, success, errors);
    Tests.Test(TestIsDateString, success, errors);
    Tests.Test(TestIsValidDate, success, errors);
    Tests.Test(TestParse, success, errors);
    Tests.Summarize(title, success, errors);
END DatesTest.

DatesTest
=========

Test the date record and procedures in the Date module.


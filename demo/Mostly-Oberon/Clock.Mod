(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
 MODULE Clock;

PROCEDURE GetRtcTime*(VAR second, minute, hour, day, month, year : INTEGER);
BEGIN
END GetRtcTime;

PROCEDURE Get*(VAR time, date : INTEGER);
BEGIN
END Get;

END Clock.

Clock
=====

This is a C time library wrapper for getting system time
to support Dates.Mod. The procedures are read only as
setting time is non-standard on many Unix-like systems[^1].
The two procedures follow the A2 style procedure signatures
adjusted for Oberon-7.


[^1]: Eric Raymond discusses time functions, http://www.catb.org/esr/time-programming/


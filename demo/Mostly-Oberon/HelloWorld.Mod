(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE HelloWorld;
  IMPORT Out;
BEGIN
  Out.String("Hello World!"); Out.Ln;
END HelloWorld.
This program isn't very useful. It has no interactive ability.
It'd be nice if it could be more specific about who it was saying
hello to.
(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
 MODULE Noises;
  IMPORT Out;

TYPE 
  Noise = PROCEDURE(who : ARRAY OF CHAR);

PROCEDURE BarkBark(who : ARRAY OF CHAR);
BEGIN
  Out.String(who);
  Out.String(": Bark, bark");Out.Ln();
END BarkBark;

PROCEDURE ChirpChirp(who : ARRAY OF CHAR);
BEGIN
  Out.String(who);
  Out.String(": Chirp, chirp");Out.Ln();
END ChirpChirp;

PROCEDURE MakeNoise(name : ARRAY OF CHAR; noise : Noise);
BEGIN
  (* Call noise with the animal name *)
  noise(name);
END MakeNoise;

BEGIN
  MakeNoise("Fido", BarkBark);
  MakeNoise("Tweety", ChirpChirp);
  MakeNoise("Fido", ChirpChirp);
  MakeNoise("Tweety", BarkBark);
END Noises.


(* @Author R. S. Doiel, <rsdoiel@gmail.com>
 * Copyright (c) 2020, All Rights Reserved, https://rsdoiel.github.io/blog/2020/04/11/Mostly-Oberon.html
 * Released under the BSD 2-clause license: https://opensource.org/licenses/BSD-2-Clause
 *)
MODULE Noises;
IMPORT Out;

TYPE
  Noise = PROCEDURE(who: ARRAY OF CHAR);
  Creature = RECORD
               name: ARRAY 32 OF CHAR;
               noise: Noise;
             END;

VAR dog, bird: Creature;

PROCEDURE BarkBark(who: ARRAY OF CHAR);
BEGIN
  Out.String(who);
  Out.String(": Bark, bark"); Out.Ln;
END BarkBark;

PROCEDURE ChirpChirp(who: ARRAY OF CHAR);
BEGIN
  Out.String(who);
  Out.String(": Chirp, chirp"); Out.Ln;
END ChirpChirp;

PROCEDURE MakeNoise(creature: Creature);
BEGIN
  (* Call noise with the animal name *)
  creature.noise(creature.name);
END MakeNoise;

PROCEDURE MakeCreature(name: ARRAY OF CHAR; noise: Noise; VAR creature: Creature);
BEGIN
  creature.name := name;
  creature.noise := noise;
END MakeCreature;

BEGIN
  MakeCreature("Fido", BarkBark, dog);
  MakeCreature("Tweety", ChirpChirp, bird);
  MakeNoise(dog);
  MakeNoise(bird);
END Noises.
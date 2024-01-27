#!/bin/zsh

for file in *.Mod; do
  ./oberon-lang -I.:./include -L.:./lib -loberon -r "$file"
done

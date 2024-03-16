#!/bin/zsh

for file in *.Mod; do
  ./oberon-lang -I.:./include -L.:./lib -loberon -r "$file"
  code=$?
  if [ $code -ne 0 ]; then
    echo "$file: [1m[91merror:[97m finished with exit code $code.[0m" >&2
  fi
done
#rm *.smb

@echo off

for %%f in (*.Mod) do (
    ..\..\build\src\oberon-lang.exe -I.;./include -L.;./lib -loberon -r %%f
    if %errorlevel% neq 0 (
        echo %%f: error: finished with exit code %errorlevel%. >&2
    )
)

del /q *.smb
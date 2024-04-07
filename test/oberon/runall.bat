@echo off

for %%f in (*.Mod) do (
    rem ..\..\build\src\oberon-lang.exe -I.:./include -L.:./lib -loberon -r %%f
    ..\..\build\src\oberon-lang.exe -I.;./include -L.;./lib -loberon -r %%f
    if %errorlevel% neq 0 (
        echo %%f: error: finished with exit code %errorlevel%. >&2
    )
)

rem del /q *.smb
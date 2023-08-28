@echo off

set CPU_ARCH=%PROCESSOR_ARCHITECTURE%
if %CPU_ARCH%==AMD64 (
	set CPU_ARCH=x64
)

nmake /nologo /f win64.make %*
# Compiler and linker settings
O7C = ..\..\build\src\oberon-lang.exe
LD = link.exe
INC = ".;.\include"

.PRECIOUS: .s .ll
.SUFFIXES: .ll .s .Mod

clean:
	@del /q *.ilk *.pdb *.obj *.exe *.s *.ll *.smb

.Mod.s:
	@$(O7C) --filetype=asm $<

.Mod.ll:
	@$(O7C) --filetype=ll $<

.Mod.obj:
	$(O7C) -O3 -I$(INC) -fenable-main $<

.Mod.exe:
	@make.bat $*.obj
	@$(LD) /nologo $*.obj /incremental:no /machine:$(CPU_ARCH) /subsystem:console /nodefaultlib:libcmt lib\oberon.lib msvcrt.lib legacy_stdio_definitions.lib

O7C = ..\..\out\build\x64-Release\src\oberon-lang.exe
LINK = link.exe
LIB = lib.exe
LDIR = .\lib

OPTFLAGS = -O3

.PRECIOUS: .s .ll
.SUFFIXES: .ll .s .Mod

clean:
	@del /q *.ilk *.pdb *.obj *.exe *.s *.ll *.bc

.Mod.s:
	@$(O7C) --filetype=asm $<

.Mod.ll:
	@$(O7C) --filetype=ll $<

.Mod.obj:
	@$(O7C) $(OPTFLAGS) $<

.Mod.exe:
	@$(MAKE) $*.obj
	@$(LINK) /nologo $*.obj /incremental:no /machine:x64 /subsystem:console ./lib/liboberon.lib msvcrt.lib legacy_stdio_definitions.lib 

liboberon: Oberon.obj Out.obj Random.obj
	@$(LIB) /nologo /machine:x64 /out:$(LDIR)\liboberon.lib Oberon.obj Out.obj Random.obj

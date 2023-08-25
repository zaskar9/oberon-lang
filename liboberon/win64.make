# Compiler settings
O7C = ..\out\build\$(PROCESSOR_ARCHITECTURE)-Release\src\oberon-lang.exe
CXX = cl.exe
LIBX = lib.exe
LIB = ..\test\oberon\lib
INC = ..\test\oberon\include

# Library settings
NAME = liboberon
EXT = lib

.PRECIOUS:
.SUFFIXES: .Mod

all: lib inc

clean:
	@del /q *.ilk *.pdb *.obj *.exe

.c.obj:
	@$(CXX) /nologo /c $< >nul

.Mod.obj:
	@$(O7C) -q -O3 $<

.smb.Mod:
	@$(O7C) -q -O3 $<

lib: runtime.obj Oberon.obj Out.obj Random.obj Math.obj
	@$(LIBX) /nologo /machine:$(PROCESSOR_ARCHITECTURE) /out:$(LIB)\$(NAME).$(EXT) runtime.obj Oberon.obj Out.obj Random.obj Math.obj

inc: Oberon.smb Out.smb Random.smb Math.smb
	@move Oberon.smb $(INC) >nul
	@move Out.smb $(INC) >nul
	@move Random.smb $(INC) >nul
	@move Math.smb $(INC) >nul

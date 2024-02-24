# Compiler settings
O7C = ..\build\src\oberon-lang.exe
CXX = cl.exe
LIBX = lib.exe
LINK = link.exe
LIB = ..\test\oberon\lib
INC = ..\test\oberon\include

O7CFLAGS = -q -O3 --reloc=pic -fenable-extern -fenable-varargs

# Library settings
NAME = oberon
EXT = lib

.PRECIOUS:
.SUFFIXES: .Mod

all: lib inc clean

clean:
	@del /q *.ilk *.pdb *.obj *.exe

.c.obj:
	@$(CXX) /nologo /c $< >nul

.Mod.obj:
	@$(O7C) $(O7CFLAGS) $<

.smb.Mod:
	@$(O7C) $(O7CFLAGS) $<

lib: runtime.obj Oberon.obj Out.obj Random.obj Math.obj
	@$(LIBX) /nologo /machine:$(CPU_ARCH) /out:$(LIB)\$(NAME).$(EXT) runtime.obj Oberon.obj Out.obj Random.obj Math.obj

inc:
	@move Oberon.smb $(INC) >nul
	@move Out.smb $(INC) >nul
	@move Random.smb $(INC) >nul
	@move Math.smb $(INC) >nul

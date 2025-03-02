# Compiler settings
O7C = ..\build\src\Release\oberon-lang.exe
CXX = cl.exe
LIBX = lib.exe
LINK = link.exe
LIB = ..\test\oberon\lib
INC = ..\test\oberon\include

O7CFLAGS = -c -O3 --reloc=pic -fenable-extern -fenable-varargs

# Library settings
NAME = oberon
EXT = lib

.PRECIOUS:
.SUFFIXES: .Mod

all: lib inc clean

clean:
	@del /q *.ilk *.pdb *.obj *.exe

.c.obj:
	@$(CXX) /nologo /GS- /c $< >nul

.Mod.obj:
	@$(O7C) $(O7CFLAGS) $<

.smb.Mod:
	@$(O7C) $(O7CFLAGS) $<

lib: runtime.obj Oberon.obj Math.obj Reals.obj Texts.obj Random.obj Out.obj
	@$(LIBX) /nologo /machine:$(CPU_ARCH) /out:$(LIB)\$(NAME).$(EXT) runtime.obj Oberon.obj Math.obj Reals.obj Texts.obj Random.obj Out.obj

inc:
	@move Math.smb $(INC) >nul
	@move Oberon.smb $(INC) >nul
	@move Out.smb $(INC) >nul
	@move Random.smb $(INC) >nul
	@move Reals.smb $(INC) >nul
	@move Texts.smb $(INC) >nul

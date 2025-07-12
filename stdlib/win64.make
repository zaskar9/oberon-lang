# Compiler settings
O7C = ..\build\olang\Release\oberon-lang.exe
CXX = cl.exe
LIBX = lib.exe
# LINK = link.exe
LIB_DIR = ..\test\oberon\lib
INC_DIR = ..\test\oberon\include

O7CFLAGS = -q -c -O3 --reloc=pic -fenable-extern -fenable-varargs

# Library settings
NAME = oberon
EXT = lib

.PRECIOUS:
.SUFFIXES: .Mod

all: lib inc clean

clean:
	@del /q *.ilk *.pdb *.obj *.exe *.exp

.c.obj:
	@$(CXX) /nologo /GS- /c $< >nul

.Mod.obj:
	@$(O7C) $(O7CFLAGS) $<

.smb.Mod:
	@$(O7C) $(O7CFLAGS) $<

lib: runtime.obj Oberon.obj Math.obj Reals.obj Texts.obj Random.obj Out.obj Files.obj
	@$(CXX) /nologo /D_USRDLL /D_WINDLL runtime.obj Oberon.obj Math.obj Reals.obj Texts.obj Random.obj Out.obj Files.obj /MT /link /DLL /out:oberon.dll
	@move oberon.dll $(LIB_DIR) >nul
	@move oberon.lib $(LIB_DIR) >nul
	@$(LIBX) /nologo runtime.obj Oberon.obj Math.obj Reals.obj Texts.obj Random.obj Out.obj Files.obj /out:oberon-static.lib
	@move oberon-static.lib $(LIB_DIR) >nul

inc:
	@move Math.smb $(INC_DIR) >nul
	@move Oberon.smb $(INC_DIR) >nul
	@move Out.smb $(INC_DIR) >nul
	@move Random.smb $(INC_DIR) >nul
	@move Reals.smb $(INC_DIR) >nul
	@move Texts.smb $(INC_DIR) >nul
	@move Files.smb $(INC_DIR) >nul

# Check operating system
ifeq ($(OS), Windows_NT)
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME), Darwin)
		OS = macos
	else ifeq ($(UNAME), Linux)
		OS = linux
		PLATFORM := $(shell uname -p)
	else
		$(error OS not supported by this Makefile)
	endif
endif

# Compiler and linker settings
# TGT = aarch64-linux-gnu
# TGT = x86_64-linux-gnu
LIB = ../test/oberon/lib
INC = ../test/oberon/include
ifeq ($(OS), windows)
	O7C = ../build/olang/oberon-lang.exe
else
	O7C = ../build/olang/oberon-lang
endif
ifeq ($(OS), windows)
	O7CFLAGS = -q -c -O3 -fenable-extern -fenable-varargs # --target $(TGT)
else
	O7CFLAGS = -q -c -O3 --reloc=pic -fenable-extern -fenable-varargs # --target $(TGT)
	ifeq ($(PLATFORM), aarch64)
      O7CFLAGS += -fno-stack-protector
    endif
endif
LDFLAGS =

# Library settings
LIBNAME = liboberon

ifeq ($(OS), windows)
	CXX = clang
	EXT = dll
	LDFLAGS += -shared
else ifeq ($(OS), macos)
	EXT = dylib
	CXX = clang
	LDFLAGS += -dynamiclib -install_name @rpath/$(LIBNAME).$(EXT)
else ifeq ($(OS), linux)
	EXT = so
	CXX = gcc # x86_64-linux-gnu-gcc
	LDFLAGS += -shared
endif

.PHONY: all clean dist install
.DEFAULT_GOAL = install

all : dist install clean

clean :
	@rm -f *.o *.smb *.$(EXT)

%.o : %.c
	@$(CXX) -c $<

%.o : %.Mod
	@$(O7C) $(O7CFLAGS) $<

%.smb : %.Mod
	@$(O7C) $(O7CFLAGS) $<

dist : runtime.o Oberon.o  Math.o Reals.o Texts.o Random.o Out.o Files.o
	@echo -n Building library...
	@$(CXX) $(LDFLAGS) -o $(LIBNAME).$(EXT) runtime.o Oberon.o Math.o Reals.o Texts.o Random.o Out.o Files.o
	@ar rcs $(LIBNAME).a runtime.o Oberon.o Files.o Math.o Reals.o Texts.o Random.o Out.o
	@echo ' done.'

install : dist Files.smb Oberon.smb Out.smb Random.smb Math.smb Reals.smb Texts.smb
	@echo -n Installing library...
	@mkdir -p $(LIB)
	@mv $(LIBNAME).$(EXT) $(LIB)
	@mv $(LIBNAME).a $(LIB)
	@echo ' done.'
	@echo -n Installing symbol files...
	@mkdir -p $(INC)
	@mv Files.smb $(INC)
	@mv Math.smb $(INC)
	@mv Oberon.smb $(INC)
	@mv Out.smb $(INC)
	@mv Random.smb $(INC)
	@mv Reals.smb $(INC)
	@mv Texts.smb $(INC)
	@echo ' done.'

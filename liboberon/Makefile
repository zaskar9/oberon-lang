# Check operating system
ifeq ($(OS), Windows_NT)
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME), Darwin)
		OS = macos
	else ifeq ($(UNAME), Linux)
		OS = linux
	else
		$(error OS not supported by this Makefile)
	endif
endif

# Compiler and linker settings
LIB = ../test/oberon/lib
INC = ../test/oberon/include
O7C = ../cmake-build-release/src/oberon-lang
O7CFLAGS = -q -O3 --reloc=pic
LDFLAGS =

# Library settings
LIBNAME = liboberon

ifeq ($(OS), macos)
	EXT = dylib
	CXX = clang
	LDFLAGS += -dynamiclib -install_name @rpath/$(LIBNAME).$(EXT)
else ifeq ($(OS), linux)
	EXT = so
	CXX = gcc
	LDFLAGS += -shared
endif

.PHONY: all clean dist install
.DEFAULT_GOAL = dist

all : dist install

clean :
	@rm -f *.o *.smb *.$(EXT)

%.o : %.c
	@$(CXX) -c $<

%.o : %.Mod
	@$(O7C) $(O7CFLAGS) $<

%.smb : %.Mod
	@$(O7C) $(O7CFLAGS) $<

dist : runtime.o Oberon.o Out.o Random.o Math.o
	@echo -n Building library...
	@$(CXX) $(LDFLAGS) -o $(LIBNAME).$(EXT) runtime.o Oberon.o Out.o Random.o Math.o
	@echo ' done.'

install : dist Oberon.smb Out.smb Random.smb Math.smb
	@echo -n Installing library...
	@mv $(LIBNAME).$(EXT) $(LIB)
	@echo ' done.'
	@echo -n Installing symbol files...
	@mv Oberon.smb $(INC)
	@mv Out.smb $(INC)
	@mv Random.smb $(INC)
	@mv Math.smb $(INC)
	@echo ' done.'
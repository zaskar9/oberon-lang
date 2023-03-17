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

# Oberon compiler settings
O7C = ../cmake-build-release/src/oberon-lang
LIB = ../test/oberon/lib
INC = ../test/oberon/include

# C++ compiler settings
CXX =

# Linker flags
LDFLAGS =

# Library extension
LIBNAME = liboberon
EXT =

ifeq ($(OS), macos)
	EXT = dylib
	CXX = clang
	LDFLAGS += -dynamiclib -install_name @rpath/$(LIBNAME).$(EXT)
else ifeq ($(OS), linux)
	EXT = so
	CXX = gcc
	LDFLAGS += -shared
endif

.PHONY: clean

all : lib inc

clean :
	@rm -f *.o *.smb *.$(EXT)

% : %.o
	@$(CXX) $< -o $@

%.o : %.Mod
	@$(O7C) -q -O3 $<

%.o : %.c
	@$(CXX) -c $<

%.smb : %.Mod
	@$(O7C) $(O7CFLAGS) $<

lib : runtime.o Oberon.o Out.o Random.o Math.o
	@$(CXX) $(LDFLAGS) -o $(LIB)/$(LIBNAME).$(EXT) runtime.o Oberon.o Out.o Random.o Math.o

inc : Oberon.smb Out.smb Random.smb Math.smb
	@mv Oberon.smb $(INC)
	@mv Out.smb $(INC)
	@mv Random.smb $(INC)
	@mv Math.smb $(INC)
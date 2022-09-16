OC = ../cmake-build-release/src/oberon-lang
CC = clang
LIB = ../test/oberon/lib
INC = ../test/oberon/include

OPTFLAGS = -O3

UNAME_S := $(shell uname)

.PHONY: clean

all : lib inc

clean :
	@rm -f *.o *.smb *.so *.dylib

% : %.o
	@$(CC) $< -o $@

%.o : %.Mod
	@$(OC) $(OPTFLAGS) $<

%.o : %.c
	@$(CC) -c $<

%.smb : %.Mod
	@$(OC) $(OPTFLAGS) $<

lib : compat.o Oberon.o Out.o Random.o
	@$(CC) -dynamiclib -o $(LIB)/liboberon.dylib compat.o Oberon.o Out.o Random.o
	@$(CC) -shared -o $(LIB)/liboberon.so compat.o Oberon.o Out.o Random.o

inc : Oberon.smb Out.smb Random.smb
	@mv Oberon.smb $(INC)
	@mv Out.smb $(INC)
	@mv Random.smb $(INC)
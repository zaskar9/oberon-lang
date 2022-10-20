OC = ../cmake-build-release/src/oberon-lang
CC = clang
LIB = ../test/oberon/lib
INC = ../test/oberon/include

.PHONY: clean

all : lib inc

clean :
	@rm -f *.o *.smb *.so *.dylib

% : %.o
	@$(CC) $< -o $@

%.o : %.Mod
	@$(OC) -q -O3 $<

%.o : %.c
	@$(CC) -c $<

%.smb : %.Mod
	@$(OC) $(O7CFLAGS) $<

lib : compat.o Oberon.o Out.o Random.o Math.o
	@$(CC) -dynamiclib -o $(LIB)/liboberon.dylib compat.o Oberon.o Out.o Random.o Math.o -install_name @rpath/liboberon.dylib
	@$(CC) -shared -o $(LIB)/liboberon.so compat.o Oberon.o Out.o Random.o Math.o -install_name @rpath/liboberon.dylib

inc : Oberon.smb Out.smb Random.smb Math.smb
	@mv Oberon.smb $(INC)
	@mv Out.smb $(INC)
	@mv Random.smb $(INC)
	@mv Math.smb $(INC)
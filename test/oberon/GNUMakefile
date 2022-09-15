O7C = ../../cmake-build-release/src/oberon-lang
CXX = clang
OPT = opt
LLC = llc
LIB = ./lib

OPTFLAGS = -O3

UNAME_S := $(shell uname)

.PHONY: clean
.PRECIOUS: %.s %.ll

clean:
	@rm -f *.s *.ll *.o *.bc # *.smb *.sym
ifeq ($(UNAME_S), Darwin)
	@find . -depth 1 -type f -perm +111 -delete
endif
ifeq ($(UNAME_S), Linux)
	@find . -type f -executable -delete
endif

% : %.o
	@$(CXX) $< -o $@ -L$(LIB) -loberon

%.o : %.Mod
	@$(O7C) $(OPTFLAGS) $<
#	@$(CXX) -c $<

%.s : %.Mod
	@$(O7C) --filetype=asm $<

%.ll : %.Mod
	@$(O7C) --filetype=ll $<

liboberon : Oberon.o Out.o Random.o
	@$(CXX) -dynamiclib -o $(LIB)/liboberon.dylib Oberon.o Out.o Random.o
	@$(CXX) -shared -o $(LIB)/liboberon.so Oberon.o Out.o Random.o
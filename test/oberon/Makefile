O7C = ../../cmake-build-release/src/oberon-lang
LNK = clang
LIB = ./lib
INC = ./include

UNAME_S := $(shell uname)

.PHONY: clean
.PRECIOUS: %.s %.ll

clean:
	@rm -f *.s *.o *.ll *.bc *.smb
ifeq ($(UNAME_S), Darwin)
	@find . -depth 1 -type f -perm +111 -delete
endif
ifeq ($(UNAME_S), Linux)
	@find . -type f -executable -delete
endif

% : %.o
	@$(LNK) $< -o $@ -L$(LIB) -loberon -rpath @executable_path/lib

%.o : %.Mod
	@$(O7C) -q -O3 -I$(INC) $<

%.s : %.Mod
	@$(O7C) --filetype=asm $<

%.ll : %.Mod
	@$(O7C) --filetype=ll $<

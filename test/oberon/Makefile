O0C = ../../cmake-build-debug/src/oberon-lang
GXX = clang
OPT = opt
LLC = llc

OPTFLAGS = -O3

UNAME_S := $(shell uname)

.PHONY: clean
.PRECIOUS: %.s %.ll

clean:
	@rm -f *.s *.ll *.o *.bc
ifeq ($(UNAME_S), Darwin)
	@find . -type f -perm +111 -delete
endif
ifeq ($(UNAME_S), Linux)
	@find . -type f -executable -delete
endif

% : %.s
	@$(GXX) $< -o $@

%.s : %.ll
	@$(OPT) $(OPTFLAGS) -S $< | $(LLC) $(OPTFLAGS) -o $@

%.ll : %.Mod
	@$(O0C) --filetype=ll $<
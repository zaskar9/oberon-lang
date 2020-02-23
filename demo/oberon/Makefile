O0C = ../../cmake-build-debug/oberon0c
GXX = g++
OPT = opt
LLC = llc

OPTFLAGS = -O3

.PHONY: clean
.PRECIOUS: %.s %.ll

clean:
	@rm -f *.s *.ll
	@find . -type f -perm +111 -delete 

% : %.s
	@$(GXX) $< -o $@

%.s : %.ll
	@$(OPT) $(OPTFLAGS) -S $< | $(LLC) $(OPTFLAGS) -o $@

%.ll : %.Mod
	@$(O0C) $<
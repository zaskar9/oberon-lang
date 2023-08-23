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
LIB = ./lib
INC = ".;./include"
O7C = ../../cmake-build-release/src/oberon-lang
O7CFLAGS = -O3 -I$(INC)
LDFLAGS = -loberon
ifeq ($(OS), macos)
    CXX = clang
	LDFLAGS += -rpath @executable_path/lib
else ifeq ($(OS), linux)
	CXX = gcc
	O7CFLAGS += --reloc=pic
	LDFLAGS += -lm -Wl,-rpath,$(LIB)
endif

.PHONY: clean
.PRECIOUS: %.s %.ll

clean:
	@rm -f *.s *.o *.ll *.bc *.smb
ifeq ($(OS), macos)
	@find . -depth 1 -type f -perm +111 -delete
else ifeq ($(OS), linux)
	@find . -maxdepth 1 -type f -executable -delete
endif

% : %.o
	@$(CXX) $< -o $@ -L$(LIB) $(LDFLAGS)

%.o : %.Mod
	@$(O7C) $(O7CFLAGS) $<

%.s : %.Mod
	@$(O7C) $(O7CFLAGS) --filetype=asm $<

%.ll : %.Mod
	@$(O7C) $(O7CFLAGS) --filetype=ll $<

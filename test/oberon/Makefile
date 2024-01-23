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
# TGT = aarch64-linux-gnu
# TGT = x86_64-linux-gnu
LIB = ./lib
INC = ".:./include"
O7C = ../../cmake-build-release/src/oberon-lang
O7CFLAGS = -O3 -I$(INC) -L$(LIB) -loberon  # --target $(TGT)
LDFLAGS = -loberon
ifeq ($(OS), macos)
	CXX = clang
	# CXXFLAGS += -target $(TGT)
	LDFLAGS += -rpath @executable_path/lib
else ifeq ($(OS), linux)
	CXX = gcc # x86_64-linux-gnu-gcc
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
	@$(CXX) $(CXXFLAGS) $< -o $@ -L$(LIB) $(LDFLAGS)

%.o : %.Mod
	@$(O7C) $(O7CFLAGS) $<

%.s : %.Mod
	@$(O7C) $(O7CFLAGS) --filetype=asm $<

%.ll : %.Mod
	@$(O7C) $(O7CFLAGS) --filetype=ll $<

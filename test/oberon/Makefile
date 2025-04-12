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
INC = .:./include
O7C = ./oberon-lang
O7CFLAGS = -c -O3 -I$(INC) -L$(LIB) -loberon -fenable-main # --target $(TGT)
LDFLAGS = -loberon
PFX =
ifeq ($(OS), macos)
	CXX = clang
	# CXXFLAGS += -target $(TGT)
	LDFLAGS += -rpath @executable_path/lib
	PFX = _
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
	@find . -depth 1 -type f ! -name "*.*" -perm +111 -delete
else ifeq ($(OS), linux)
	@find . -maxdepth 1 -type f ! -name "*.*" -executable -delete
endif

% : %.o
	@$(CXX) $(CXXFLAGS) $< -o $@ -L$(LIB) $(LDFLAGS) # -Wl,-e,$(PFX)$(basename $<)

%.o : %.Mod
	@$(O7C) $(O7CFLAGS) $<

%.s : %.Mod
	@$(O7C) $(O7CFLAGS) -S $<

%.ll : %.Mod
	@$(O7C) $(O7CFLAGS) -S --emit-llvm $<

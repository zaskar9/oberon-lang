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
O7C = ../../cmake-build-release/src/oberon-lang
LIB = ./lib
INC = ./include

# C++ compiler settings
CXX = clang

# Linker flags
LDFLAGS = -loberon
ifeq ($(OS), macos)
	LDFLAGS += -rpath @executable_path/lib
else ifeq ($(OS), linux)
	LDFLAGS += -lm
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
	@$(O7C) -O3 -I$(INC) $<

%.s : %.Mod
	@$(O7C) --filetype=asm $<

%.ll : %.Mod
	@$(O7C) --filetype=ll $<

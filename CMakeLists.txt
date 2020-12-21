cmake_minimum_required(VERSION 3.17)

project(oberon-llvm)

if (MSVC)
    # warning level 4
    add_compile_options(/Wall /W4 /w44265 /w44061 /w44062 /experimental:external /external:W0 /external:anglebrackets)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-W -Wall -Wextra -Werror -Wpedantic -Wunreachable-code -Wshadow -Woverloaded-virtual -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wsign-conversion -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

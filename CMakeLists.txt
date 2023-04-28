cmake_minimum_required(VERSION 3.26)

project(oberon-lang VERSION 0.0.1)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # warning level 4
    add_compile_options(/Wall /W4 /w44265 /w44061 /w44062 /experimental:external /external:W0 /external:anglebrackets)
else ()
    # lots of warnings and all warnings as errors -Wglobal-constructors -Wexit-time-destructors
    add_compile_options(-W -Wall -Wextra -Werror -Wpedantic -Wunreachable-code -Wshadow -Woverloaded-virtual -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wsign-conversion)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wweak-vtables)
        # add_link_options(-fuse-ld=lld)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# define DEBUG
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif ()

# add the cmake folder of the project for extension modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(src)
# add_subdirectory(docs)

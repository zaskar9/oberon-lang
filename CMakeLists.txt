cmake_minimum_required(VERSION 3.30)

project(oberon-lang VERSION 0.2.0)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(WARNING "No build type set, defaulting to \"Release\".")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # warning level 4
    add_compile_options(/Wall /W4 /external:anglebrackets /external:W0 /utf-8)
else ()
    # lots of warnings and all warnings as errors -Wglobal-constructors -Wexit-time-destructors
    add_compile_options(-W -Wall -Wextra -Werror -Wpedantic -Wno-error=deprecated -Wunreachable-code -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wsign-conversion -Wfloat-conversion)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wweak-vtables)
        if (WIN32)
            add_compile_options(-fms-extensions)
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # using GCC
    endif ()
endif ()

# compile error for LLVM 16 with C++23
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Introduce an option to toggle between static and shared libraries
option(BUILD_SHARED_LIBS "Build and Link against shared libraries." OFF)

# define DEBUG
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif ()

# add the cmake folder of the project for extension modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(LLVM REQUIRED CONFIG)
if (LLVM_FOUND)
    if (${LLVM_VERSION} LESS 16.0.0)
        message(WARNING "Found legacy LLVM: ${LLVM_VERSION}")
        add_definitions(-D_LLVM_LEGACY)
    else ()
        message(STATUS "Found LLVM: ${LLVM_VERSION}")
        if (${LLVM_VERSION} MATCHES "16.*.*")
            add_definitions(-D_LLVM_16)
        elseif (${LLVM_VERSION} MATCHES "17.*.*")
            add_definitions(-D_LLVM_17)
        elseif (${LLVM_VERSION} MATCHES "18.*.*")
            add_definitions(-D_LLVM_18)
        elseif (${LLVM_VERSION} MATCHES "19.*.*")
            add_definitions(-D_LLVM_19)
        elseif (${LLVM_VERSION} MATCHES "20.*.*")
            add_definitions(-D_LLVM_20)
        endif()
    endif ()
    add_definitions(${LLVM_DEFINITIONS})
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    llvm_map_components_to_libnames(llvm_libs core executionengine jitlink orcjit nativecodegen support passes ${LLVM_TARGETS_TO_BUILD})
endif ()

find_package(Boost REQUIRED COMPONENTS program_options)
if (Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_VERSION}")
    set(Boost_USE_MULTITHREADED TRUE)
    set(Boost_NO_WARN_NEW_VERSIONS 1)
endif ()

#find_package(BDWgc QUIET)
#if (BDWgc_FOUND)
#    message(STATUS "Found BDWgc: ${BDWgc_VERSION}")
#endif ()

# Add source tree to search path to avoid relative includes
# include_directories(BEFORE .)

add_subdirectory(lib)
add_subdirectory(olang)
add_subdirectory(utils/not)
add_subdirectory(utils/grammar)
add_subdirectory(stdlib)
add_subdirectory(test)
add_subdirectory(docs)

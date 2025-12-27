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
    # add_compile_options(-fsanitize=undefined)
    # add_link_options(-fsanitize=undefined)
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
        message(WARNING "Found legacy LLVM: ${LLVM_INCLUDE_DIRS} (found version \"${LLVM_VERSION}\")")
        add_definitions(-D_LLVM_LEGACY)
    else ()
        message(STATUS "Found LLVM: ${LLVM_INCLUDE_DIRS} (found version \"${LLVM_VERSION}\")")
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
        elseif (${LLVM_VERSION} MATCHES "21.*.*")
            add_definitions(-D_LLVM_21)
        endif()
    endif ()
    # Check for dynamic library to avoid conflict with dynamically linked LLD
    if (LLVM_LINK_LLVM_DYLIB)
        set(llvm_libs LLVM)
        message(STATUS "Linking to dynamic LLVM library.")
    else ()
        llvm_map_components_to_libnames(llvm_libs Core Support TargetParser Passes OrcJIT ExecutionEngine Object ${LLVM_TARGETS_TO_BUILD})
        message(STATUS "Linking to static LLVM libraries: ${llvm_libs}")
    endif ()
    # Add LLVM compiler definitions
    add_definitions(${LLVM_DEFINITIONS})
    # Display all targets that will be included in the build
    message(STATUS "Including LLVM Targets: ${LLVM_TARGETS_TO_BUILD}")
    # Make sure LLD is found if installed relative to LLVM
    list(APPEND CMAKE_PREFIX_PATH ${LLVM_INSTALL_PREFIX})
endif ()

find_package(LLD REQUIRED CONFIG)
if (LLD_FOUND)
    message(STATUS "Found LLD: ${LLD_INCLUDE_DIRS} (found version \"${LLD_VERSION}\")")
#    find_library(LLD_COFF NAMES liblldCOFF.dylib liblldCOFF.so lldCOFF.lib lldCOFF.a liblldCOFF.dll.a liblldCOFF.a REQUIRED)
#    find_library(LLD_COMMON NAMES liblldCommon.dylib liblldCommon.so lldCommon.lib lldCommon.a liblldCommon.dll.a liblldCommon.a REQUIRED)
#    find_library(LLD_ELF NAMES liblldELF.dylib liblldELF.so lldELF.lib lldELF.a liblldELF.dll.a liblldELF.a REQUIRED)
#    if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
#        find_library(LLD_MACHO NAMES liblldMachO.dylib liblldMachO.so lldMachO.lib lldMachO.a liblldMachO.dll.a liblldMachO.a REQUIRED)
#    else()
#        set(LLD_MACHO "")
#    endif()
#    find_library(LLD_MINGW NAMES liblldMinGW.dylib liblldMinGW.so lldMinGW.lib lldMinGW.a liblldMinGW.dll.a liblldMinGW.a REQUIRED)
#    find_library(LLD_WASM NAMES liblldWasm.dylib liblldWasm.so lldWasm.lib lldWasm.a liblldWasm.dll.a liblldWasm.a REQUIRED)
#    set(lld_libs ${LLD_COFF} ${LLD_WASM} ${LLD_MINGW} ${LLD_ELF} ${LLD_MACHO} ${LLD_COMMON})
    set(lld_libs lldCommon lldCOFF lldELF lldMachO lldMinGW lldWasm)
    message(STATUS "Linking to LLD libraries: ${lld_libs}")
endif ()

find_package(Boost REQUIRED COMPONENTS headers program_options)
if (Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS} (found version \"${Boost_VERSION}\")")
    set(Boost_USE_MULTITHREADED TRUE)
    set(Boost_NO_WARN_NEW_VERSIONS FALSE)
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

set(LIBOBERON "oberon")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OBFLAGS -q -O3 --reloc=pic -fenable-extern -fenable-varargs)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(OBFLAGS -q -O3 --reloc=pic -fenable-extern -fenable-varargs)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(OBFLAGS -q -O3 -fenable-extern -fenable-varargs)
endif()

# add source tree to search path to avoid relative includes
include_directories(BEFORE .)

function(add_oberon_module target depends)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.o
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.smb
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../src/oberon-lang 
            -I${CMAKE_CURRENT_BINARY_DIR} 
            ${OBFLAGS}
            --sym-dir=${CMAKE_CURRENT_BINARY_DIR}
            -o ${CMAKE_CURRENT_BINARY_DIR}/${target}.o
            ${CMAKE_CURRENT_SOURCE_DIR}/${target}.Mod
    DEPENDS ${depends}
    )
endfunction()

add_oberon_module(Oberon "")
add_oberon_module(Reals "")
add_oberon_module(Texts "${CMAKE_CURRENT_BINARY_DIR}/Reals.o")
add_oberon_module(Out "")
add_oberon_module(Random "")
add_oberon_module(Math "")

add_library(${LIBOBERON}dyn SHARED
    runtime.c
    ${CMAKE_CURRENT_BINARY_DIR}/Oberon.o
    ${CMAKE_CURRENT_BINARY_DIR}/Reals.o
    ${CMAKE_CURRENT_BINARY_DIR}/Texts.o
    ${CMAKE_CURRENT_BINARY_DIR}/Out.o
    ${CMAKE_CURRENT_BINARY_DIR}/Random.o
    ${CMAKE_CURRENT_BINARY_DIR}/Math.o
)

add_library(${LIBOBERON} STATIC
    runtime.c
    ${CMAKE_CURRENT_BINARY_DIR}/Oberon.o
    ${CMAKE_CURRENT_BINARY_DIR}/Reals.o
    ${CMAKE_CURRENT_BINARY_DIR}/Texts.o
    ${CMAKE_CURRENT_BINARY_DIR}/Out.o
    ${CMAKE_CURRENT_BINARY_DIR}/Random.o
    ${CMAKE_CURRENT_BINARY_DIR}/Math.o
)

install(TARGETS ${LIBOBERON} ${LIBOBERON}dyn
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/Oberon.smb
    ${CMAKE_CURRENT_BINARY_DIR}/Reals.smb
    ${CMAKE_CURRENT_BINARY_DIR}/Texts.smb
    ${CMAKE_CURRENT_BINARY_DIR}/Out.smb
    ${CMAKE_CURRENT_BINARY_DIR}/Random.smb
    ${CMAKE_CURRENT_BINARY_DIR}/Math.smb
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/oberon-lang
)
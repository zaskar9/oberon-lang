set(LIBOBERON_SHARED "liboberon-shared")
set(LIBOBERON_STATIC "liboberon-static")
set(LIBOBERON_NAME "oberon")

set(O_EXT ${CMAKE_C_OUTPUT_EXTENSION})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(OBFLAGS -c -q -O3 --reloc=pic -fenable-extern -fenable-varargs)
  if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(OBFLAGS ${OBFLAGS} -fno-stack-protector)
  endif()
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(OBFLAGS -c -q -O3 --reloc=pic -fenable-extern -fenable-varargs)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(OBFLAGS -c -q -O3 -fenable-extern -fenable-varargs)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(LIBOBERON_NAME_STATIC "oberon-static")
else()
    set(LIBOBERON_NAME_STATIC ${LIBOBERON_NAME})
endif()

# add source tree to search path to avoid relative includes
include_directories(BEFORE .)

function(add_oberon_module target depends)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}${O_EXT}
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.smb
        COMMAND $<TARGET_FILE:oberon-lang>
        -I${CMAKE_CURRENT_BINARY_DIR}
        ${OBFLAGS}
        --sym-dir=${CMAKE_CURRENT_BINARY_DIR}
        -o ${CMAKE_CURRENT_BINARY_DIR}/${target}${O_EXT}
        ${CMAKE_CURRENT_SOURCE_DIR}/${target}.Mod
        DEPENDS ${depends}
    )
endfunction()

add_oberon_module(Oberon "")
add_oberon_module(Reals "")
add_oberon_module(Texts "${CMAKE_CURRENT_BINARY_DIR}/Reals${O_EXT}")
add_oberon_module(Out "${CMAKE_CURRENT_BINARY_DIR}/Texts${O_EXT}")
add_oberon_module(Random "${CMAKE_CURRENT_BINARY_DIR}/Oberon${O_EXT}")
add_oberon_module(Math "")

add_library(${LIBOBERON_SHARED} SHARED
    runtime.c
    ${CMAKE_CURRENT_BINARY_DIR}/Oberon${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Reals${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Texts${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Out${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Random${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Math${O_EXT}
)

set_target_properties(
  ${LIBOBERON_SHARED} PROPERTIES
  OUTPUT_NAME ${LIBOBERON_NAME}
)

add_library(${LIBOBERON_STATIC} STATIC
    runtime.c
    ${CMAKE_CURRENT_BINARY_DIR}/Oberon${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Reals${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Texts${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Out${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Random${O_EXT}
    ${CMAKE_CURRENT_BINARY_DIR}/Math${O_EXT}
)

set_target_properties(
  ${LIBOBERON_STATIC} PROPERTIES
  OUTPUT_NAME ${LIBOBERON_NAME_STATIC}
  ARCHIVE_OUTPUT_DIRECTORY static
)

install(TARGETS ${LIBOBERON_STATIC} ${LIBOBERON_SHARED}
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

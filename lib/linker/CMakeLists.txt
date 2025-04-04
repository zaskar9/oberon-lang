set(OLANG_LINK "olang-linker" CACHE INTERNAL "Target for linker.")
set(OLANG_LINK_NAME "OlangLink")

set(OLANG_LINK_SOURCES
        LLDWrapper.cpp LLDWrapper.h)

if (BUILD_SHARED_LIBS)
    add_library(${OLANG_LINK} STATIC ${OLANG_LINK_SOURCES})
else ()
    add_library(${OLANG_LINK} SHARED ${OLANG_LINK_SOURCES})
endif ()

set_target_properties(${OLANG_LINK} PROPERTIES OUTPUT_NAME ${OLANG_LINK_NAME})

target_include_directories(${OLANG_LINK} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

# Include internal dependencies
target_link_libraries(${OLANG_LINK} PUBLIC global)
target_link_libraries(${OLANG_LINK} PRIVATE ${OLANG_LOG})


if (LLVM_FOUND)
    target_LINK_libraries(${OLANG_LINK} PRIVATE ${llvm_libs})
endif ()
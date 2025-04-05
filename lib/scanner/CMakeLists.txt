set(OLANG_LEX "olang-scanner" CACHE INTERNAL "Target for scanner.")
set(OLANG_LEX_NAME "OlangScanner")
set(OLANG_LEX_SOURCES
        Scanner.cpp Scanner.h
        Token.cpp Token.h
        LiteralToken.cpp LiteralToken.h
        IdentToken.cpp IdentToken.h
        UndefinedToken.cpp UndefinedToken.h)

if (BUILD_SHARED_LIBS)
    add_library(${OLANG_LEX} SHARED ${OLANG_LEX_SOURCES})
else ()
    add_library(${OLANG_LEX} STATIC ${OLANG_LEX_SOURCES})
endif ()

set_target_properties(${OLANG_LEX} PROPERTIES OUTPUT_NAME ${OLANG_LEX_NAME})

target_include_directories(${OLANG_LEX} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

# Include internal dependencies
target_link_libraries(${OLANG_LEX} PUBLIC global)
target_link_libraries(${OLANG_LEX} PRIVATE ${OLANG_LOG})

# Include external dependencies
if (Boost_FOUND)
    target_link_libraries(${OLANG_LEX} PRIVATE Boost::headers)
endif ()


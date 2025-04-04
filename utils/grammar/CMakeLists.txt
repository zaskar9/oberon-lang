option(USE_STATIC_LIBS "Link against static libraries" ON)

set(GRAMMAR_UTIL "olang-grammar")
set(GRAMMAR_UTIL_SOURCES
        Grammar.cpp Grammar.h
        Validator.cpp Validator.h
        Loader.cpp Loader.h)

configure_file(config.h.in config.h @ONLY)

add_executable(${GRAMMAR_UTIL} main.cpp ${GRAMMAR_UTIL_SOURCES})
install(TARGETS ${GRAMMAR_UTIL} RUNTIME DESTINATION bin)

target_include_directories(${GRAMMAR_UTIL} PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})

# Include internal dependencies
target_link_libraries(${GRAMMAR_UTIL} PUBLIC global)
target_link_libraries(${GRAMMAR_UTIL} PRIVATE ${OLANG_LEX} ${OLANG_LOG})

# Include external dependencies
if (Boost_FOUND)
    target_link_libraries(${GRAMMAR_UTIL} PRIVATE Boost::headers)
endif ()



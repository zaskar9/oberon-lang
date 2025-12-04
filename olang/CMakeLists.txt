set(OLANG_FRONTEND "olang-frontend")
set(OLANG_FRONTEND_NAME "oberon-lang")
set(OLANG_FRONTEND_SOURCES main.cpp)

configure_file(config.h.in config.h @ONLY)

add_executable(${OLANG_FRONTEND} ${OLANG_FRONTEND_SOURCES})
install(TARGETS ${OLANG_FRONTEND} RUNTIME DESTINATION bin)

set_target_properties(${OLANG_FRONTEND} PROPERTIES OUTPUT_NAME ${OLANG_FRONTEND_NAME})
# Main executable needs to export symbols (of statically linked LLVM) for JIT mode
set_target_properties(${OLANG_FRONTEND} PROPERTIES ENABLE_EXPORTS ON)

target_include_directories(${OLANG_FRONTEND} PUBLIC . ${CMAKE_CURRENT_BINARY_DIR})

# Include internal dependencies
target_link_libraries(${OLANG_FRONTEND} PUBLIC global)
target_link_libraries(${OLANG_FRONTEND} PRIVATE ${OLANG_BULK} ${OLANG_LEX} ${OLANG_LOG})

# Include external dependencies
if (Boost_FOUND)
    target_include_directories(${OLANG_FRONTEND} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(${OLANG_FRONTEND} PRIVATE Boost::headers Boost::program_options)
endif ()


# Check that lit binary is in path.
find_program(TEST_SUITE_LIT NAMES "lit" "llvm-lit")
set(TEST_SUITE_LIT_FLAGS --ignore-fail -s CACHE STRING "Flags used when running lit.")

# Check that filecheck binary is in path.
find_program(TEST_SUITE_FILECHECK NAMES "filecheck")

if(TEST_SUITE_LIT-NOTFOUND OR TEST_SUITE_FILECHECK-NOTFOUND)
  message(WARNING "Skipping unit tests: lit or filecheck not found.")
else()
  add_custom_target(test COMMAND ${TEST_SUITE_LIT} ${TEST_SUITE_LIT_FLAGS} --config-prefix=$<CONFIG> ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
  )
  # Custom target depends on compiler and library target.
  add_dependencies(test oberon-lang liboberon-static liboberon-shared)
  # Set up the configuration file.
  configure_file(lit.site.cfg.py.in lit.site.cfg.tmp @ONLY)
  file(GENERATE OUTPUT $<CONFIG>.site.cfg.py INPUT ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.tmp)
endif()
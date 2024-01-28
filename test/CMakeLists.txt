find_program(TEST_SUITE_LIT NAMES "lit" "llvm-lit")
set(TEST_SUITE_LIT_FLAGS "-s -v" CACHE STRING "Flags used when running lit")
configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)
add_custom_target(test COMMAND ${TEST_SUITE_LIT} ${TEST_SUITE_LIT_FLAGS} .
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  USES_TERMINAL
                  )
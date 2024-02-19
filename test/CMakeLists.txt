# TODO : Depends on compiler and library target exists
find_program(TEST_SUITE_LIT NAMES "lit" "llvm-lit")
set(TEST_SUITE_LIT_FLAGS "-s -v" CACHE STRING "Flags used when running lit")

find_program(TEST_SUITE_FILECHECK NAMES "filecheck")

if(TEST_SUITE_LIT-NOTFOUND OR TEST_SUITE_FILECHECK-NOTFOUND)
        message(WARNING "Skipping unittests: lit or filecheck not found.")
else()
        configure_file(lit.site.cfg.py.in lit.site.cfg.py @ONLY)
        add_custom_target(test COMMAND ${TEST_SUITE_LIT} ${TEST_SUITE_LIT_FLAGS} .
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        USES_TERMINAL
        )
endif()
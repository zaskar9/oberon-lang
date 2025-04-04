set(UTIL_NOT "olang-not")
set(UTIL_NOT_SOURCES main.cpp)

add_executable(${UTIL_NOT} main.cpp)
set_target_properties(${UTIL_NOT} PROPERTIES OUTPUT_NAME "not")
install(TARGETS ${UTIL_NOT} RUNTIME DESTINATION bin)
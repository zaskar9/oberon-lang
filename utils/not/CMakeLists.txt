set(UTIL_NOT "not")
set(UTIL_NOT_SOURCES main.cpp)

add_executable(${UTIL_NOT} main.cpp)
install (TARGETS ${UTIL_NOT} RUNTIME DESTINATION bin)
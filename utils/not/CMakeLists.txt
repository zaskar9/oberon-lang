set(NOT_UTIL "olang-not")
set(NOT_UTIL_NAME "not")
set(NOT_UTIL_SOURCES main.cpp)

add_executable(${NOT_UTIL} main.cpp)
set_target_properties(${NOT_UTIL} PROPERTIES OUTPUT_NAME ${NOT_UTIL_NAME})
install(TARGETS ${NOT_UTIL} RUNTIME DESTINATION bin)
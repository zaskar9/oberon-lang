set(OLANG_LOG "olang-logger" CACHE INTERNAL "Target for logger.")
set(OLANG_LOG_NAME "OlangLogger")
set(OLANG_LOG_SOURCES
        Logger.cpp Logger.h)

if (BUILD_SHARED_LIBS)
    add_library(${OLANG_LOG} SHARED ${OLANG_LOG_SOURCES})
else ()
    add_library(${OLANG_LOG} STATIC ${OLANG_LOG_SOURCES})
endif ()

set_target_properties(${OLANG_LOG} PROPERTIES OUTPUT_NAME ${OLANG_LOG_NAME})

target_include_directories(${OLANG_LOG} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>)

# Include internal dependencies
target_link_libraries(${OLANG_LOG} PUBLIC global)

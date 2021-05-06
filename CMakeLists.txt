cmake_minimum_required(VERSION 3.0)
project(clean_path)

include(CheckIncludeFile)

set(CMAKE_C_STANDARD 99)
set(LIB_SOURCES lib/cleanpath.c)
set(MAIN_SOURCES src/main.c)
set(INCLUDES include/cleanpath.h)

configure_file ("${PROJECT_SOURCE_DIR}/config.h.in"
                "${PROJECT_BINARY_DIR}/include/config.h")

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

include(CTest)
enable_testing()
add_subdirectory(tests)

add_library(cleanpath_static STATIC ${LIB_SOURCES})
set_target_properties(cleanpath_static PROPERTIES PUBLIC_HEADER ${INCLUDES})
set_target_properties(cleanpath_static PROPERTIES OUTPUT_NAME cleanpath)

add_library(cleanpath_shared SHARED ${LIB_SOURCES})
set_target_properties(cleanpath_shared PROPERTIES PUBLIC_HEADER ${INCLUDES})
set_target_properties(cleanpath_shared PROPERTIES OUTPUT_NAME cleanpath)

add_executable(cleanpath ${MAIN_SOURCES})
target_link_libraries(cleanpath cleanpath_static)

install(TARGETS cleanpath_static
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib)

install(TARGETS cleanpath_shared
        PUBLIC_HEADER DESTINATION include
        LIBRARY DESTINATION lib)

install(TARGETS cleanpath
        RUNTIME DESTINATION bin)

install(FILES ${CMAKE_SOURCE_DIR}/docs/man/cleanpath.1
        DESTINATION share/man/man1 COMPONENT doc)

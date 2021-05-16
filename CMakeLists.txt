cmake_minimum_required(VERSION 3.0)
project(cleanpath)

include(CheckIncludeFile)
include(CheckSymbolExists)
include(CTest)

set(CMAKE_C_STANDARD 99)
set(LIB_SOURCES lib/cleanpath.c)
set(MAIN_SOURCES src/main.c)
set(LIB_INCLUDES include/cleanpath/cleanpath.h)

check_symbol_exists(confstr "unistd.h" HAVE_CONFSTR)

configure_file ("${PROJECT_SOURCE_DIR}/config.h.in"
                "${PROJECT_BINARY_DIR}/include/config.h")

include_directories(${PROJECT_BINARY_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

enable_testing()
add_subdirectory(tests)

add_library(cleanpath_static STATIC ${LIB_SOURCES})
set_target_properties(cleanpath_static PROPERTIES PUBLIC_HEADER ${LIB_INCLUDES})
set_target_properties(cleanpath_static PROPERTIES OUTPUT_NAME cleanpath)

add_library(cleanpath_shared SHARED ${LIB_SOURCES})
set_target_properties(cleanpath_shared PROPERTIES PUBLIC_HEADER ${LIB_INCLUDES})
set_target_properties(cleanpath_shared PROPERTIES OUTPUT_NAME cleanpath)

add_executable(cleanpath ${MAIN_SOURCES})
target_link_libraries(cleanpath cleanpath_static)

install(TARGETS cleanpath_static
        PUBLIC_HEADER DESTINATION include/cleanpath
        LIBRARY DESTINATION lib)

install(TARGETS cleanpath_shared
        PUBLIC_HEADER DESTINATION include/cleanpath
        LIBRARY DESTINATION lib)

install(TARGETS cleanpath
        RUNTIME DESTINATION bin)


file(GLOB MAN1_FILES ${CMAKE_SOURCE_DIR}/docs/man/*.1)
file(GLOB MAN3_FILES ${CMAKE_SOURCE_DIR}/docs/man/*.3)

foreach(file ${MAN1_FILES})
    install(FILES ${file}
            DESTINATION share/man/man1 COMPONENT doc)
endforeach()

foreach(file ${MAN3_FILES})
    install(FILES ${file}
            DESTINATION share/man/man3 COMPONENT doc)
endforeach()

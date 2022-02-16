include_directories(
		${CMAKE_SOURCE_DIR}/include
		${CMAKE_BINARY_DIR}/include
)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
set(PROJECT_LIBS cleanpath_static)

file(GLOB files "test_*.c")

foreach(file ${files})
	string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
	add_executable(${file_without_ext} ${file})
	if(NOT(MSVC))
		target_compile_options(${file_without_ext} PRIVATE -Wno-unused-parameter)
	endif()
	target_link_libraries(${file_without_ext} ${PROJECT_LIBS})
	add_test(${file_without_ext} ${file_without_ext})
	set_tests_properties(${file_without_ext}
		PROPERTIES
		TIMEOUT 120)
endforeach()

# For these tests to pass TESTVAR_WIN (Windows) and TESTVAR_NIX (Unix-like)
# must be set in the runtime environment prior to execution. For some
# reason environment variables defined here are not passed on to child
# processes.
set(TEST_SETUP -E TESTVAR)
set(TEST_SETUP_ALL --all)
set(TEST_SETUP_ALL_LIST --all-list)

if(WIN32)
    set(TEST_SETUP ${TEST_SETUP}_WIN)
	set(exec ${CMAKE_BINARY_DIR}/cleanpath.exe)

	add_test(cmd_arg_exact ${exec} --exact ${TEST_SETUP} C:/usr/bin)
	add_test(cmd_arg_loose ${exec} --loose ${TEST_SETUP} C:/usr/bin)
else()
	set(TEST_SETUP ${TEST_SETUP}_NIX)
	set(exec ${CMAKE_BINARY_DIR}/cleanpath)

	add_test(cmd_arg_exact ${exec} ${TEST_SETUP} --exact /usr/bin)
	add_test(cmd_arg_loose ${exec} ${TEST_SETUP} --loose /usr/bin)
	add_test(cmd_arg_regex ${exec} ${TEST_SETUP} --regex /usr/bin)
	add_test(cmd_arg_all_exact ${exec} ${TEST_SETUP_ALL} --exact /usr/bin)
	add_test(cmd_arg_all_loose ${exec} ${TEST_SETUP_ALL} --loose /usr/bin)
	add_test(cmd_arg_all_regex ${exec} ${TEST_SETUP_ALL} --regex /usr/bin)
	add_test(cmd_arg_all_exact ${exec} ${TEST_SETUP_ALL_LIST} --exact /usr/bin)
	add_test(cmd_arg_all_loose ${exec} ${TEST_SETUP_ALL_LIST} --loose /usr/bin)
	add_test(cmd_arg_all_regex ${exec} ${TEST_SETUP_ALL_LIST} --regex /usr/bin)
endif()

add_test(cmd_arg_help ${exec} --help)
add_test(cmd_arg_version ${exec} --version)
add_test(cmd_arg_list ${exec} --list)
add_test(cmd_arg_default ${exec} --default)

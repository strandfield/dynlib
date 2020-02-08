
enable_testing()

add_library(testlib SHARED "lib.cpp")

add_executable(test "main.cpp")
target_link_libraries(test dynlib)
target_include_directories(test PUBLIC "../include")

add_dependencies(test testlib)

add_test(NAME test COMMAND test)

enable_testing()

add_library(testlib SHARED "lib.cpp")
set_target_properties(testlib PROPERTIES PREFIX "")

add_executable(test_dynlib "main.cpp")
target_link_libraries(test_dynlib dynlib)
target_include_directories(test_dynlib PUBLIC "../include")

add_dependencies(test_dynlib testlib)

add_test(NAME test_dynlib COMMAND test_dynlib)

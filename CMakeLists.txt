cmake_minimum_required(VERSION 3.9)
project(dynlib)

set (CMAKE_CXX_STANDARD 11)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # disables warning about DLL export 
  # this is annoying when using C++ STL
  add_compile_options(/wd4251)  
  add_compile_options(/wd4275) 
endif()

##################################################################
###### dynlib
##################################################################

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HDR_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
add_library(dynlib ${HDR_FILES} ${SRC_FILES})
target_include_directories(dynlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (NOT WIN32)
  target_link_libraries(dynlib ${CMAKE_DL_LIBS})
endif()

foreach(_source IN ITEMS ${HDR_FILES} ${SRC_FILES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

target_compile_definitions(dynlib PUBLIC -DDYNLIB_COMPILE_LIBRARY)

##################################################################
###### tests, examples & benchmarks
##################################################################

add_subdirectory(tests)

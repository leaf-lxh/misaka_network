cmake_minimum_required (VERSION 3.5)
set (CMAKE_CXX_STANDARD 11)

aux_source_directory (. SOURCE_INTERFACE)
add_library (interfaces ${SOURCE_INTERFACE})

target_link_libraries (interfaces tinyHttpd)

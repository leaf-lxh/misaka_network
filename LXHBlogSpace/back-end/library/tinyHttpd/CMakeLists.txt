cmake_minimum_required (VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)
aux_source_directory (. SOURCE_TINYHTTPD)
add_library (tinyHttpd ${SOURCE_TINYHTTPD})

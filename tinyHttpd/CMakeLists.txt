cmake_minimum_required (VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

project (tinyHttpd)

aux_source_directory (. SCRS_ROOT)

add_executable (tinyHttpd ${SCRS_ROOT})

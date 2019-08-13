cmake_minimum_required (VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

project (tinyLogger)

aux_source_directory(. SOURCES_ROOT)

add_executable(tinyLogger ${SOURCES_ROOT})


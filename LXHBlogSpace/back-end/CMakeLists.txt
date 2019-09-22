cmake_minimum_required (VERSION 3.5)
project (blogspace_backend)
set (CMAKE_CXX_STANDARD 11)

#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/release/bin/")
add_subdirectory(app)
add_subdirectory(library)


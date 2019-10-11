#The cmake file for passport app.
cmake_minimum_required (VERSION 3.5)
set (CMAKE_CXX_STANDARD 11)

#Include the library that being used in this app.
include_directories(BEFORE "${CMAKE_SOURCE_DIR}/library/")
add_subdirectory(interfaces)

#For building this app: 
aux_source_directory (. SOURCE_ROOT)
add_executable (blogspace_backend_member ${SOURCE_ROOT})
target_link_libraries (blogspace_backend_member member_interfaces)

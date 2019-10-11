#This file is mean to define the apps that included in "blogspace_backend" project.
#By adding "add_subdirectory(folder)" to add your app.
cmake_minimum_required (VERSION 3.5)
set (CMAKE_CXX_STANDARD 11)

add_subdirectory(passport)
add_subdirectory(content)
add_subdirectory(member)
cmake_minimum_required (VERSION 3.5)

project (auto_sign_in)

add_subdirectory(modules)

aux_source_directory (. SCRS_ROOT)

add_executable (auto_sign_in ${SCRS_ROOT})

#Modify if your lib directory is not this.
set(MYSQL_LIB_DIR /usr/lib/x86_64-linux-gnu)

link_directories(${MYSQL_LIB_DIR})
target_link_libraries (auto_sign_in mysqlclient)
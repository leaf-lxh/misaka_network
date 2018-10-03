cmake_minimum_required (VERSION 3.5)

aux_source_directory (. SRCS_MODULES)
add_library(modules ${SRCS_MODULES})

#Modify if this don't match your path of libmysqlclient.so. 
set(MYSQL_LIB_DIR /usr/lib/x86_64-linux-gnu)
link_directories(${MYSQL_LIB_DIR})
target_link_libraries (modules mysqlclient)
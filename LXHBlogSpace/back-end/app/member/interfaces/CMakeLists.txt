cmake_minimum_required (VERSION 3.5)
set (CMAKE_CXX_STANDARD 11)

aux_source_directory (. SOURCE_INTERFACE)
add_library (member_interfaces ${SOURCE_INTERFACE})

#mysqlcppconn dependences:
set(MYSQL_LIB_DIR /usr/lib/)
link_directories(${MYSQL_LIB_DIR})
target_link_libraries (member_interfaces mysqlcppconn)

#tinyHttpd
target_link_libraries (member_interfaces tinyHttpd)
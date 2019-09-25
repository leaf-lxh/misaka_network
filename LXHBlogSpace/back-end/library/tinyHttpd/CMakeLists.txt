cmake_minimum_required (VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)
aux_source_directory (. SOURCE_TINYHTTPD)
add_library (tinyHttpd ${SOURCE_TINYHTTPD})

#for openssl dependences
set(OPENSSL_LIB_DIR /usr/local/lib/)
link_directories(${OPENSSL_LIB_DIR})
target_link_libraries (tinyHttpd crypto)

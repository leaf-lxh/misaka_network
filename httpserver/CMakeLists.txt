#需要的CMAKE最低版本
cmake_minimum_required (VERSION 3.5)

#当前项目名称
project (httpserver)

#引入当前目录所有的源码文件
aux_source_directory (. SRCS_ROOT_DIR)

#添加子目录
add_subdirectory (HTTPHandler)
add_subdirectory (script)

#生成目标文件
add_executable (httpserver ${SRCS_ROOT_DIR})

#指明编译时需要链接的库
target_link_libraries (httpserver LIBS_HTTPHandler)
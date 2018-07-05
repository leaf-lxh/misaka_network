cmake_minimum_required (VERSION 3.5)

project (auto_sign_in)

add_subdirectory(modules)

aux_source_directory (. SCRS_ROOT)

add_executable (auto_sign_in ${SCRS_ROOT})

target_link_libraries (auto_sign_in LIBS_MODULES)
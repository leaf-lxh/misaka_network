cmake_minimum_required (VERSION 3.5)

aux_source_directory (. SCRS_HTTPHandler)

add_library (LIBS_HTTPHandler ${SCRS_HTTPHandler})
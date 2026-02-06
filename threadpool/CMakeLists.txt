cmake_minimum_required(VERSION 3.15)

project(threadPool)

aux_source_directory(./ SRC)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

add_library(threadpool STATIC ${SRC})

target_include_directories(threadpool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)

target_link_libraries(threadpool PRIVATE pthread)
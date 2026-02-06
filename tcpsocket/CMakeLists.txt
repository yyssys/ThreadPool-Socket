cmake_minimum_required(VERSION 3.15)

project(tcpSocket)

aux_source_directory(./ SRC)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

add_library(tcpsocket STATIC ${SRC})

target_include_directories(tcpsocket PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)
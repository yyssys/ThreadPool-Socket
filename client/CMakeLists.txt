cmake_minimum_required(VERSION 3.15)

project(client)

aux_source_directory(./ SRC)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

add_executable(client ${SRC})

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc)

target_link_libraries(client PRIVATE tcpsocket)
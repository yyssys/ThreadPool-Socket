cmake_minimum_required(VERSION 3.15)

project(tinyServer)

add_subdirectory(threadpool)

add_subdirectory(tcpsocket)

add_subdirectory(client)

add_subdirectory(server)
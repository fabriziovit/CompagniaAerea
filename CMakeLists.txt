cmake_minimum_required(VERSION 3.16)
project(CompagniaAerea C)

set(CMAKE_C_STANDARD 99)

add_executable(CompagniaAerea main.c sqlite3.c sqlite3.h sqlite3ext.h graph.c graph.h list.h list.c list.c list.h listAeroporti.c listAeroporti.h)
cmake_minimum_required(VERSION 2.8)

project(fast-aleck)

include_directories(include)

add_definitions(-std=gnu99 -Wall -Wextra -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter)

set(
	SOURCES_EXECUTABLE
	./src/fast-aleck/fast-aleck.c
	./src/fast-aleck/fast-aleck-main.c
)

set(
	SOURCES_LIBRARY
	./src/fast-aleck/fast-aleck.c
)

set(
	SOURCES_TEST
	./src/fast-aleck/fast-aleck.c
	./src/fast-aleck/fast-aleck-test.c
)

add_executable(fast-aleck ${SOURCES_EXECUTABLE})
add_executable(fast-aleck-test ${SOURCES_TEST})
add_library(libfast-aleck SHARED ${SOURCES_LIBRARY})

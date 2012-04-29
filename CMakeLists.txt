cmake_minimum_required(VERSION 2.8)

project(fast-aleck)

include_directories(include)

add_definitions(-std=gnu99 -Wall -Wextra -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter)

set(
	SOURCES
	./src/fast-aleck/fast-aleck.c
	./src/fast-aleck/fast-aleck-main.c
)

add_executable(fast-aleck ${SOURCES})

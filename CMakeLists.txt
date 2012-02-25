cmake_minimum_required(VERSION 2.8)

project(ultrapants)

include_directories(include)

add_definitions(-std=gnu99 -Wall -Wextra -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter)

set(
	SOURCES
	./src/ultrapants/ultrapants.c
	./src/ultrapants/ultrapants-main.c
)

add_executable(ultrapants ${SOURCES})

cmake_minimum_required(VERSION 2.8)

project(ultrapants)

include_directories(include)

add_definitions(-std=gnu99 -Wall -Wextra -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter)

set(
	SOURCES
	./src/ultrapants/ultrapants.c
	./src/ultrapants-test/ultrapants-test.c
)

add_executable(ultrapants ${SOURCES})

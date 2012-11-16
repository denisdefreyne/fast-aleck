cmake_minimum_required(VERSION 2.8)

project(fast-aleck)

include_directories(
	include
)

add_definitions(-std=gnu99 -Wall -Wextra -Wno-missing-braces -Wno-unused-function -Wno-unused-parameter)

set(
	SOURCES_SHARED
	./src/fast-aleck/fast-aleck.c
	./src/fast-aleck/slice.c
	./src/fast-aleck/token.c
	./src/fast-aleck/token-buffer.c
	./src/fast-aleck/tokenizer.c
	./src/fast-aleck/text-processor.c
	./src/fast-aleck/caps-processor.c
	./src/fast-aleck/widont-processor.c
)

set(
	SOURCES_EXECUTABLE
	${SOURCES_SHARED}
	./src/fast-aleck/fast-aleck-main.c
)

set(
	SOURCES_LIBRARY
	${SOURCES_SHARED}
)

set(
	SOURCES_TEST
	${SOURCES_SHARED}
	./src/fast-aleck/test/fast-aleck-test.c
)

add_executable(fast-aleck ${SOURCES_EXECUTABLE})

add_executable(fast-aleck-test ${SOURCES_TEST})

add_library(libfast-aleck SHARED ${SOURCES_LIBRARY})
SET_TARGET_PROPERTIES(libfast-aleck PROPERTIES PREFIX "")

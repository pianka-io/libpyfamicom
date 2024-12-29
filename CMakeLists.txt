cmake_minimum_required(VERSION 3.10)

project(pytendo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fPIC -march=armv8-a")

set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR src)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

set(SOURCES
    ${SRC_DIR}/app/main.c
    ${SRC_DIR}/com/clock.c
    ${SRC_DIR}/com/globals.c
    ${SRC_DIR}/cpu/cpu.c
    ${SRC_DIR}/cpu/memory.c
    ${SRC_DIR}/cpu/opcodes.c
    ${SRC_DIR}/cpu/stack.c
    ${SRC_DIR}/emu/emu.c
    ${SRC_DIR}/ines/rom.c
)

add_library(pytendo SHARED ${SOURCES})
set_target_properties(pytendo PROPERTIES OUTPUT_NAME "libpytendo")

add_executable(pytendo_exec ${SOURCES})
set_target_properties(pytendo_exec PROPERTIES OUTPUT_NAME "pytendo")

target_include_directories(pytendo PUBLIC ${SRC_DIR})
target_include_directories(pytendo_exec PUBLIC ${SRC_DIR})

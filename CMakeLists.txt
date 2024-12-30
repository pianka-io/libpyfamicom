cmake_minimum_required(VERSION 3.10)

project(pytendo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fPIC -march=armv8-a")

set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR src)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

set(Python3_EXECUTABLE /opt/homebrew/bin/python3.13)
find_package(Python3 COMPONENTS Development NumPy REQUIRED)

message(STATUS "Python Executable: ${Python3_EXECUTABLE}")
message(STATUS "Python Include Dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python Libraries: ${Python3_LIBRARIES}")

include_directories(${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})
link_directories(${Python3_LIBRARY_DIRS})

set(SOURCES
    ${SRC_DIR}/example.c
    ${SRC_DIR}/app/main.c
    ${SRC_DIR}/com/clock.c
    ${SRC_DIR}/com/globals.c
    ${SRC_DIR}/com/interrupt.c
    ${SRC_DIR}/cpu/cpu.c
    ${SRC_DIR}/cpu/memory.c
    ${SRC_DIR}/cpu/opcodes.c
    ${SRC_DIR}/cpu/stack.c
    ${SRC_DIR}/emu/emu.c
    ${SRC_DIR}/ines/rom.c
    ${SRC_DIR}/ppu/memory.c
    ${SRC_DIR}/ppu/ppu.c
)

add_library(pytendo SHARED ${SOURCES})
set_target_properties(pytendo PROPERTIES OUTPUT_NAME "pytendo" PREFIX "" SUFFIX ".cpython-313-darwin.so")
target_include_directories(pytendo PUBLIC ${SRC_DIR})
target_link_libraries(pytendo ${Python3_LIBRARIES})
add_custom_command(
        TARGET pytendo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:pytendo>
        ${CMAKE_SOURCE_DIR}/api/
)

add_executable(pytendo_exec ${SOURCES})
set_target_properties(pytendo_exec PROPERTIES OUTPUT_NAME "pytendo")
target_include_directories(pytendo_exec PUBLIC ${SRC_DIR})
target_link_libraries(pytendo_exec ${Python3_LIBRARIES})

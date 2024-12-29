cmake_minimum_required(VERSION 3.10)

project(pyfamicom C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fPIC -march=armv8-a")

set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(SRC_DIR src)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

set(SOURCES
    ${SRC_DIR}/app/main.c
    ${SRC_DIR}/cpu/cpu.c
    ${SRC_DIR}/cpu/cpu_registers.c
    ${SRC_DIR}/com/clock.c
)

add_library(pyfamicom SHARED ${SOURCES})
set_target_properties(pyfamicom PROPERTIES OUTPUT_NAME "libpyfamicom")

add_executable(pyfamicom_exec ${SOURCES})
set_target_properties(pyfamicom_exec PROPERTIES OUTPUT_NAME "pyfamicom")

target_include_directories(pyfamicom PUBLIC ${SRC_DIR})
target_include_directories(pyfamicom_exec PUBLIC ${SRC_DIR})

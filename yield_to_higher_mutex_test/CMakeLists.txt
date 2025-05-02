# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(yield_to_higher_mutex_test)

target_sources(app PRIVATE src/main.c)
#set_compiler_property(PROPERTY optimization_speed)
#target_compile_options(app PRIVATE -O2 -g0)

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/lib/posix/options)
target_include_directories(app PRIVATE ${ZEPHYR_BASE}/kernel/include)    
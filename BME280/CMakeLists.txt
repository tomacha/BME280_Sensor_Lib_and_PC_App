cmake_minimum_required(VERSION 3.19)

set(This proj_name)

project(${This} CXX C ASM)

add_subdirectory(bsp/bsp_interface) # Add library that provides bsp common interface
add_subdirectory(bme280)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
    message("Building application for NUCLEO-L432KC")
    add_subdirectory(bsp/stm32l432kc) # Add implementation of bsp library for stm32l432kc
    set(CMAKE_EXECUTABLE_SUFFIX .elf) # Select executable extension for NUCLEO target
else()
    message(FATAL_ERROR "Processor CMAKE_SYSTEM_PROCESSOR not set.")
endif()

add_executable(${This} main.cpp )
target_link_libraries(${This} bsp bme280)
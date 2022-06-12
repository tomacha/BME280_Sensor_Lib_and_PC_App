cmake_minimum_required(VERSION 3.19)

set(This bme280_project)

project(${This} CXX C ASM)

add_subdirectory(Libraries/bme280)
add_subdirectory(Libraries/bme280_cli)
add_subdirectory(Libraries/cli)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
    message("Building application for NUCLEO-L432KC")
    add_subdirectory(bsp/bsp_interface)   
    add_subdirectory(bsp/stm32l432kc) # Add implementation of bsp library for stm32l432kc
    set(CMAKE_EXECUTABLE_SUFFIX .elf) # Select executable extension for NUCLEO target
    add_executable(${This} main.cpp )
    target_link_libraries(${This} bsp bme280 bme280_cli cli)
else()
    enable_testing()
    add_subdirectory(googletest)
    add_subdirectory(tests)
endif()

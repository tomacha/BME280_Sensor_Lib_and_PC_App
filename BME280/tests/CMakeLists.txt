cmake_minimum_required(VERSION 3.19)

set(This Testing)

set(Sources
    BME280_Tests.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    gtest_main
    bme280
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
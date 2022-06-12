add_library(bme280_cli src/bme280_cli.cpp)
target_include_directories(bme280_cli PUBLIC include)

target_link_libraries(
    bme280_cli
    PUBLIC cli
    PUBLIC bme280
    PUBLIC bsp
)
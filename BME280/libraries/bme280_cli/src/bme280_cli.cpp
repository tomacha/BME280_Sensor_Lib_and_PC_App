/**
 * @file bme280_cli.cpp
 * @authors Krzysztof Półchłopek, Michał Tomacha
 * @version 0.1
 * @date 2022-06-08
 */

#include <string>

#include "bsp.hpp"
#include "bme280_cli.hpp"
#include "usart.h"

extern BME280::bme280 sensor;
extern "C" UART_HandleTypeDef huart2;

static CLI::cli_status bme280Init(std::vector<std::string> *argv);
static CLI::cli_status bme280GetMeasurements(std::vector<std::string> *argv);

std::vector<CLI::cli_command_t> BME280_Commands{
    {.command = "bme280_init", .cmd_handler = &bme280Init},
    {.command = "bme280_getmeas", .cmd_handler = &bme280GetMeasurements},
    };

static CLI::cli_status bme280Init(std::vector<std::string> *argv)
{
    auto status = sensor.bme280_Init(BME280::OVERSAMPLING_X1,BME280::OVERSAMPLING_X1,BME280::OVERSAMPLING_X1,
                    BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF);
    
    if (status == BME280::STATUS_OK)
    {
        return CLI::STATUS_OK;
    }
    return CLI::STATUS_ERROR;
}

static CLI::cli_status bme280GetMeasurements(std::vector<std::string> *argv)
{
    std::string str;

    auto status = sensor.bme280_ReadMeasurements();
    bsp::delay(1000);

    str = "Temp: " + std::to_string(sensor.temperature) + "\n\rHum: " + std::to_string(sensor.humidity)
        + "\n\rPress: " + std::to_string(sensor.pressure) + "\n\r";
    HAL_UART_Transmit(&huart2, (uint8_t*) str.data(), str.length(), 100);

    if (status == BME280::STATUS_OK)
    {
        return CLI::STATUS_OK;
    }
    return CLI::STATUS_ERROR;
}


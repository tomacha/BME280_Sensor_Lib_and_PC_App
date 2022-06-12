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
    {.command = "bme280_initialize", .cmd_handler = &bme280Init},
    {.command = "bme280_get_measurements", .cmd_handler = &bme280GetMeasurements},
    };

static CLI::cli_status bme280Init(std::vector<std::string> *argv)
{
    BME280::meas_oversampling temp_oversampling, hum_oversampling, press_oversampling;
    BME280::working_mode mode = BME280::MODE_SLEEP;
    BME280::standby_time s_time = BME280::STANDBY_0_5;
    BME280::filter_coeffitiens filter = BME280::FILTER_OFF;

    for (auto i = 1; i < argv->size(); i++)
    {
        if (argv->at(i) == "temp:")
        {
            if (argv->at(++i) == "meas_off")
            {
                temp_oversampling = BME280::OVERSAMPLING_NONE;
            }
            else if (argv->at(i) == "meas_1x")
            {
                temp_oversampling = BME280::OVERSAMPLING_X1;
            }
            else if (argv->at(i) == "meas_2x")
            {
                temp_oversampling = BME280::OVERSAMPLING_X2;
            }
            else if (argv->at(i) == "meas_4x")
            {
                temp_oversampling = BME280::OVERSAMPLING_X4;
            }
            else if (argv->at(i) == "meas_8x")
            {
                temp_oversampling = BME280::OVERSAMPLING_X8;
            }
            else if (argv->at(i) == "meas_16x")
            {
                temp_oversampling = BME280::OVERSAMPLING_X16;
            }
        }
        else if (argv->at(i) == "hum:")
        {
            if (argv->at(++i) == "meas_off")
            {
                hum_oversampling = BME280::OVERSAMPLING_NONE;
            }
            else if (argv->at(i) == "meas_1x")
            {
                hum_oversampling = BME280::OVERSAMPLING_X1;
            }
            else if (argv->at(i) == "meas_2x")
            {
                hum_oversampling = BME280::OVERSAMPLING_X2;
            }
            else if (argv->at(i) == "meas_4x")
            {
                hum_oversampling = BME280::OVERSAMPLING_X4;
            }
            else if (argv->at(i) == "meas_8x")
            {
                hum_oversampling = BME280::OVERSAMPLING_X8;
            }
            else if (argv->at(i) == "meas_16x")
            {
                hum_oversampling = BME280::OVERSAMPLING_X16;
            }
        }
        else if (argv->at(i) == "press:")
        {
            if (argv->at(++i) == "meas_off")
            {
                press_oversampling = BME280::OVERSAMPLING_NONE;
            }
            else if (argv->at(i) == "meas_1x")
            {
                press_oversampling = BME280::OVERSAMPLING_X1;
            }/* condition */
            else if (argv->at(i) == "meas_2x")
            {
                press_oversampling = BME280::OVERSAMPLING_X2;
            }
            else if (argv->at(i) == "meas_4x")
            {
                press_oversampling = BME280::OVERSAMPLING_X4;
            }
            else if (argv->at(i) == "meas_8x")
            {
                press_oversampling = BME280::OVERSAMPLING_X8;
            }
            else if (argv->at(i) == "meas_16x")
            {
                press_oversampling = BME280::OVERSAMPLING_X16;
            }
        }
        else if (argv->at(i) == "mode_forced")
        {
            mode = BME280::MODE_FORCED;
        }
        else if (argv->at(i) == "mode_normal")
        {
            mode = BME280::MODE_NORMAL;
        }
        else if (argv->at(i) == "stime_0.5ms")
        {
            s_time = BME280::STANDBY_0_5;
        }
        else if (argv->at(i) == "stime_10ms")
        {
            s_time = BME280::STANDBY_10;
        }
        else if (argv->at(i) == "stime_20ms")
        {
            s_time = BME280::STANDBY_20;
        }
        else if (argv->at(i) == "stime_62.5ms")
        {
            s_time = BME280::STANDBY_62_5;
        }
        else if (argv->at(i) == "stime_125ms")
        {
            s_time = BME280::STANDBY_125;
        }
        else if (argv->at(i) == "stime_250ms")
        {
            s_time = BME280::STANDBY_250;
        }
        else if (argv->at(i) == "stime_500ms")
        {
            s_time = BME280::STANDBY_500;
        }
        else if (argv->at(i) == "stime_1000ms")
        {
            s_time = BME280::STANDBY_1000;
        }
        else
        {
            /* code */
        }
        
    }

    auto status = sensor.bme280_Init(temp_oversampling, hum_oversampling, press_oversampling,
                    mode, s_time, filter);
    
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

    str = "TEMP " + std::to_string(sensor.temperature) + "\nHUM " + std::to_string(sensor.humidity)
        + "\nPRESS " + std::to_string(sensor.pressure) + "\n";
    HAL_UART_Transmit(&huart2, (uint8_t*) str.data(), str.length(), 100);

    if (status == BME280::STATUS_OK)
    {
        return CLI::STATUS_OK;
    }
    return CLI::STATUS_ERROR;
}


/**
 * @file bme280_cli.hpp
 * @authors Krzysztof Półchłopek, Michał Tomacha
 * @version 0.1
 * @date 2022-06-08
 */

#ifndef _BME280_CLI_H_
#define _BME280_CLI_H_

#include "bme280.hpp"
#include "cli.hpp"
#include <vector>

extern std::vector<CLI::cli_command_t> BME280_Commands;

#endif
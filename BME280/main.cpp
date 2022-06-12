#include "bsp.hpp"

#include "usart.h"
#include "spi.h"

#include "stm32l4xx_hal_uart.h"
#include "stm32l4xx_hal_spi.h"
#include "stm32l4xx_hal.h"

#include "bme280.hpp"
#include "cli.hpp"
#include "bme280_cli.hpp"

#include <cstring>
#include <cstdio>

extern "C" UART_HandleTypeDef huart2;
extern "C" SPI_HandleTypeDef hspi1;

#define DEBUG(_msg) (HAL_UART_Transmit(&huart2, (uint8_t*) _msg, strlen(_msg), 100))

namespace BME280 {
    bme_status sensRead(uint8_t reg_addr, uint8_t *data, uint32_t len){
        reg_addr = reg_addr | 0x80;
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
        auto s1 = HAL_SPI_Transmit(&hspi1, const_cast<uint8_t*>(&reg_addr), 1, 1000);
        auto s2 = HAL_SPI_Receive(&hspi1, data, len, 1000);
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
        return (((HAL_OK == s1) && (HAL_OK == s2)) ? STATUS_OK : STATUS_ERROR);
    }

    bme_status sensWrite(uint8_t reg_addr, uint8_t *data, uint32_t len){
        reg_addr = reg_addr & 0x7F;
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
        auto s1 = HAL_SPI_Transmit(&hspi1, const_cast<uint8_t*>(&reg_addr), 1, 1000);
        auto s2 = HAL_SPI_Transmit(&hspi1, const_cast<uint8_t*>(data), len, 1000);
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
        return (((HAL_OK == s1) && (HAL_OK == s2)) ? STATUS_OK : STATUS_ERROR);
    }

    void sensDelay(uint32_t ms)
    {
        HAL_Delay(ms);
    }
};

namespace CLI {
    cli_status cliWrite(uint8_t *data, uint32_t data_len)
    {
        auto status = HAL_UART_Transmit(&huart2, (uint8_t*) data, data_len, 100);
        return ((status == HAL_OK) ? STATUS_OK : STATUS_ERROR);
    }
}

char Rx_char;
extern std::vector<CLI::cli_command_t> BME280_Commands;
CLI::cli comands(&CLI::cliWrite, &BME280_Commands, CLI::MODE_FORCED);
BME280::bme280 sensor(BME280::sensRead, BME280::sensWrite, BME280::sensDelay);

int main() {
    // HAL init
    bsp::init();
    bsp::delay(100);
    
    HAL_UART_Receive_IT(&huart2, (uint8_t*) &Rx_char, 1);
    
    while (true) {
        comands.cli_processCommand();
        bsp::delay(100);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		comands.cli_receiveChar(Rx_char);
		HAL_UART_Receive_IT(&huart2, (uint8_t*) &Rx_char, 1);
	}
}
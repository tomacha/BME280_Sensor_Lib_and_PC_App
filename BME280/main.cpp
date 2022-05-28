#include "bsp.hpp"

#include "usart.h"
#include "spi.h"

#include "stm32l4xx_hal_uart.h"
#include "stm32l4xx_hal_spi.h"
#include "stm32l4xx_hal.h"

#include "bme280.hpp"

#include <cstring>

extern "C" UART_HandleTypeDef huart2;
extern "C" SPI_HandleTypeDef hspi1;

namespace BME280 {
    bme_status sensRead(uint8_t reg_addr, uint8_t *data, uint32_t len){
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
        auto s1 = HAL_SPI_Transmit(&hspi1, const_cast<uint8_t*>(&reg_addr), 1, 1000);
        auto s  = HAL_SPI_Receive(&hspi1, data, len, 1000);
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
        return ((HAL_OK == s) && (HAL_OK == s1)) ? STATUS_OK : STATUS_ERROR ;

    }

    bme_status sensWrite(uint8_t reg_addr, uint8_t *data, uint32_t len){
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
        auto s1 = HAL_SPI_Transmit(&hspi1, const_cast<uint8_t*>(&reg_addr), 1, 1000);
        auto s2 = HAL_SPI_Transmit(&hspi1, const_cast<uint8_t*>(data), len, 1000);
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
        return ((HAL_OK == s1) && (HAL_OK == s2)) ? STATUS_OK : STATUS_ERROR ;

    }

    void sensDelay(uint32_t ms)
    {
        HAL_Delay(ms);
    }
};


int main() {
    // HAL init
    bsp::init();

    BME280::bme_status status;
    BME280::bme280 sensor(BME280::sensRead, BME280::sensWrite, BME280::sensDelay);

    HAL_UART_Transmit(&huart2, (uint8_t*) "TEST1\n",strlen("TEST1\n"),100);
    status = sensor.bme280_Init(BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,BME280::OVERSAMPLING_NONE,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF);
    HAL_UART_Transmit(&huart2, (uint8_t*) "TEST2\n",strlen("TEST2\n"),100);
    
    
    while (true) {     
        
        if (status == BME280::STATUS_OK)
        {
            HAL_UART_Transmit(&huart2, (uint8_t*) "OK\n",strlen("OK\n"),100);
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            bsp::delay(1000);
        }
        else
        {
            HAL_UART_Transmit(&huart2, (uint8_t*) "ERROR\n",strlen("ERROR\n"),100);
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            bsp::delay(100);
        }
    }
}

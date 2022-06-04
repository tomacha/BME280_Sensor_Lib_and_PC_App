#include "bsp.hpp"

#include "usart.h"
#include "spi.h"

#include "stm32l4xx_hal_uart.h"
#include "stm32l4xx_hal_spi.h"
#include "stm32l4xx_hal.h"

#include "bme280.hpp"

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
        auto s2  = HAL_SPI_Receive(&hspi1, data, len, 1000);
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


int main() {
    // HAL init
    bsp::init();
    bsp::delay(100);

    char table[30];
    uint32_t len;

    BME280::bme_status status;
    BME280::bme280 sensor(BME280::sensRead, BME280::sensWrite, BME280::sensDelay);

    DEBUG("TEST1\n\r");
    status = sensor.bme280_Init(BME280::OVERSAMPLING_X1,BME280::OVERSAMPLING_X1,BME280::OVERSAMPLING_X1,
        BME280::MODE_SLEEP,BME280::STANDBY_0_5,BME280::FILTER_OFF);
    DEBUG("TEST2\n\r");
    DEBUG(((status == BME280::STATUS_OK) ? "STATUS_OK\n\r" : "STATUS_ERR\n\r"));
    
    
    while (true) {
        status = sensor.bme280_ReadMeasurements();
        DEBUG(((status == BME280::STATUS_OK) ? "STATUS_OK\n\r" : "STATUS_ERR\n\r"));
        bsp::delay(1000);
        len = sprintf(table,"Temp: %.1f\n\r", sensor.temperature);
        HAL_UART_Transmit(&huart2, (uint8_t*) &table, len, 100);
        len = sprintf(table,"Hum: %.1f\n\r", sensor.humidity);
        HAL_UART_Transmit(&huart2, (uint8_t*) &table, len, 100);
        len = sprintf(table,"Press: %.1f\n\r", sensor.pressure);
        HAL_UART_Transmit(&huart2, (uint8_t*) &table, len, 100);
    }
}

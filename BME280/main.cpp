#include "bsp.hpp"

#include "usart.h"
#include "stm32l4xx_hal_uart.h"
#include "bme280.hpp"

#include <cstring>

extern "C" UART_HandleTypeDef huart2;

int main() {
    char hello[] = "Hello world!\r\n";
    bsp::init();
    HAL_UART_Transmit(&huart2, (uint8_t *)hello, strlen(hello), 100);

    while (true) {     
        bsp::delay(1000);
    }
}

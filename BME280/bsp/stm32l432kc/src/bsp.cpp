#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"


extern "C" void SystemClock_Config(void);
namespace bsp {


    void init() {
        HAL_Init();
        SystemClock_Config();
        MX_GPIO_Init();
        MX_SPI1_Init();
        MX_USART2_UART_Init();
    }


} // namespace bsp

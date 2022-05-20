#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "usart.h"


extern "C" void SystemClock_Config(void);
namespace bsp {


    void init() {
        HAL_Init();
        SystemClock_Config();
        MX_GPIO_Init();
        MX_I2C1_Init();
        MX_USART2_UART_Init();
    }


} // namespace bsp

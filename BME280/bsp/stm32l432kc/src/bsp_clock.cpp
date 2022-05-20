#include "bsp_clock.hpp"

#include "stm32l4xx_hal.h"
namespace bsp {

    void delay(int duration) {
        HAL_Delay(duration);
    }


} // namespace bsp
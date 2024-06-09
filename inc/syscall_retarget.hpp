#pragma once
#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#ifdef __cplusplus
#include <functional>

void SyscallUARTRetarget(UART_HandleTypeDef* huart, uint32_t timeout, std::function<void()> onTxStart, std::function<void()> onTxEnd);

extern "C"
{
#endif

int IsRetargeted();

#ifdef __cplusplus
}
#endif

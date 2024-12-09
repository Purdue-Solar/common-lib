/**
 * @file timer_helpers.hpp
 * @author Purdue Solar Racing
 * @brief Commmon timer helper functions
 * @version 0.1
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t GetTimerInputFrequency(TIM_TypeDef* tim);
void SetTimerFrequency(TIM_TypeDef* tim, uint32_t frequency, uint32_t precision);

#ifdef __cplusplus
}
#endif
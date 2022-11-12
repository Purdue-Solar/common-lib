/**
 * @file delay.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief 
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __DELAY_H
#define __DELAY_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32/auto_include.h"

namespace PSR
{
	/**
	 * @brief Wait for a specific amount of microseconds before continuing.
	 * 
	 * @param timer1MHz A reference to a 1MHz timer.
	 * @param microseconds The number of microseconds to delay.
	 */
	void delayMicroseconds(TIM_HandleTypeDef* timer1MHz, uint32_t microseconds);
}

#endif __DELAY_H // End of include guard for delay.h
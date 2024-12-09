/**
 * @file pin.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Wraps a GPIO port and pin for easy manipulation
 * @version 0.1
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#include <cstdint>

namespace PSR
{

class GpioPin
{
  private:
	GPIO_TypeDef* const port;	/// @brief The GPIO port
	const uint16_t pin;			/// @brief The pin bitmask

  public:
	
	constexpr GpioPin(GPIO_TypeDef* port, uint16_t pin)
		: port(port), pin(pin)
	{}

	void Set()
	{
		port->BSRR = pin;
	}

	void Reset()
	{
		port->BRR = pin;
	}

	void SetValue(bool value)
	{
		if (value)
			Set();
		else
			Reset();
	}

	void Toggle()
	{
		port->ODR ^= pin;
	}

	bool IsSet() const
	{
		return (port->IDR & pin) != 0;
	}
};

} // namespace PSR
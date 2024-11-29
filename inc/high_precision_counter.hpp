#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_rcc.h)

#include <cstdint>

namespace PSR
{

class HighPrecisionCounter
{
  private:
	TIM_HandleTypeDef* const htim;
	const uint32_t timerPrecision;
	uint32_t upperCount;

	static HighPrecisionCounter* instance;

	static void TimerCallback(TIM_HandleTypeDef* htim);

	static uint32_t GetTimerInputFrequency(TIM_TypeDef* tim)
	{
		RCC_ClkInitTypeDef clkConfig;
		uint32_t latency;
		uint32_t frequency;

		HAL_RCC_GetClockConfig(&clkConfig, &latency);

#ifdef APB3PERIPH_BASE
		if ((size_t)tim >= (size_t)APB3PERIPH_BASE) {
			frequency = HAL_RCC_GetPCLK3Freq();
			if (clkConfig.APB3CLKDivider != RCC_HCLK_DIV1)
				frequency *= 2;
		}
		else
#endif
#ifdef APB2PERIPH_BASE
			if ((size_t)tim >= (size_t)APB2PERIPH_BASE)
		{
			frequency = HAL_RCC_GetPCLK2Freq();
			if (clkConfig.APB2CLKDivider != RCC_HCLK_DIV1)
				frequency *= 2;
		}
		else
#endif
			if ((size_t)tim >= (size_t)APB1PERIPH_BASE)
		{
			frequency = HAL_RCC_GetPCLK1Freq();
			if (clkConfig.APB1CLKDivider != RCC_HCLK_DIV1)
				frequency *= 2;
		}
		else
			frequency = HAL_RCC_GetHCLKFreq();

		return frequency;
	}

  public:
	constexpr HighPrecisionCounter(TIM_HandleTypeDef* const htim, uint32_t timerPrecision) : htim(htim), timerPrecision(timerPrecision), upperCount(0) {}

	void Init();

	uint64_t GetCount() const;

	void Reset()
	{
		upperCount          = 0;
		htim->Instance->CNT = 0;
	}

	void Delay(int32_t microseconds)
	{
		if (microseconds < 0)
			return;

		uint64_t start = GetCount();
		uint64_t end   = start + microseconds;
		while (GetCount() < end) {
		}
	}
};

} // namespace PSR

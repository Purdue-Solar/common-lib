#include "timer_helpers.h"

uint32_t GetTimerInputFrequency(TIM_TypeDef* tim)
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

bool SetTimerFrequency(TIM_TypeDef* tim, uint32_t frequency, uint32_t precision)
{
	uint32_t inputFrequency = GetTimerInputFrequency(tim);

	uint32_t prescaler = inputFrequency / (frequency * precision);

	if (prescaler > 0x10000)
		return false;

	tim->ARR = precision - 1;
	tim->PSC = prescaler - 1;

	return true;
}
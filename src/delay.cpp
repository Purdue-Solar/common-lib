#include "delay.h"

namespace PSR
{

void delayMicroseconds(TIM_HandleTypeDef* timer1MHz, uint32_t microseconds)
{
	__HAL_TIM_SET_COUNTER(timer1MHz, 0); // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(timer1MHz) < us)
	{
	} // wait for the counter to reach the us input in the parameter
}

void delayMilliseconds(TIM_HandleTypeDef* timer1MHz, uint32_t milliseconds)
{
	delayMicroseconds(timer1MHz, 1000 * milliseconds);
}

} // namespace PSR

#include "high_precision_counter.hpp"

using namespace PSR;

HighPrecisionCounter* HighPrecisionCounter::instance = nullptr;

void HighPrecisionCounter::TimerCallback(TIM_HandleTypeDef* htim)
{
	if (instance == nullptr)
		return;
	instance->upperCount += instance->timerPrecision;
}

void HighPrecisionCounter::Init()
{
	upperCount = 0;

	HighPrecisionCounter::instance = this;

	uint32_t clockFreq = GetTimerInputFrequency(htim->Instance);

	htim->Instance->CR1         = 0;
	htim->Instance->DIER        = TIM_DIER_UIE;
	htim->Instance->PSC         = clockFreq / 1000000;
	htim->Instance->ARR         = timerPrecision - 1;
	htim->Instance->CNT         = 0xFFFFFFFF;
	htim->PeriodElapsedCallback = HighPrecisionCounter::TimerCallback;
	htim->Instance->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
}

uint64_t HighPrecisionCounter::GetCount() const
{
	return this->upperCount + this->htim->Instance->CNT;
}
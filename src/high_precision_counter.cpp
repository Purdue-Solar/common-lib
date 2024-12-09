#include "high_precision_counter.hpp"
#include "interrupt_queue.hpp"
#include "timer_helpers.h"

using namespace PSR;

void HighPrecisionCounter::Update()
{
	if (this->tim == nullptr || !this->isInitialized)
		return;

	this->upperCount += this->timerPrecision;

	this->HandleDelayCallbacks();
}

bool HighPrecisionCounter::Init()
{
	if (isInitialized)
		return true;

	upperCount         = 0;
	uint32_t clockFreq = GetTimerInputFrequency(tim);

	tim->CR1  = 0;
	tim->DIER = TIM_DIER_UIE;
	tim->PSC  = clockFreq / 1000000 - 1;
	tim->ARR  = timerPrecision - 1;
	tim->CNT  = 0xFFFFFFFF;
	tim->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;

	delayedCallbacks.fill(DelayedCallback());

	isInitialized = true;

	return true;
}

void HighPrecisionCounter::HandleDelayCallbacks()
{
	uint64_t count = GetCount();
	for (size_t i = 0; i < delayedCallbacks.size(); i++)
	{
		DelayedCallback& delayedCallback = delayedCallbacks[i];
		if (delayedCallback.Callback != nullptr && delayedCallback.DelayUntil != 0 && count >= delayedCallback.DelayUntil)
		{
			InterruptQueue::AddInterrupt(delayedCallback.Callback);

			delayedCallback.DelayUntil = 0;
			delayedCallback.Callback   = nullptr;
		}
	}
}

void HighPrecisionCounter::ClearCallbacks()
{
	for (size_t i = 0; i < delayedCallbacks.size(); i++)
	{
		delayedCallbacks[i].DelayUntil = 0;
		delayedCallbacks[i].Callback   = nullptr;
	}
}

bool HighPrecisionCounter::AddDelayedCallback(uint32_t delay, const std::function<void()>& callback)
{
	if (delay == 0 || callback == nullptr)
		return false;

	uint64_t delayUntil = GetCount() + delay * MillesecondsToMicroseconds;

	for (size_t i = 0; i < delayedCallbacks.size(); i++)
	{
		// Search for an empty slot
		DelayedCallback& delayedCallback = delayedCallbacks[i];
		if (delayedCallback.Callback == nullptr || delayedCallback.DelayUntil == 0)
		{
			delayedCallback.DelayUntil = delayUntil;
			delayedCallback.Callback   = callback;
			return true;
		}
	}

	// No empty slots
	return false;
}
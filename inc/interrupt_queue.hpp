#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#include <array>
#include <functional>
#include <queue>

namespace PSR
{

struct Interrupt
{
	std::function<void()> Function;
	uint32_t Tick;
	const char* Name;
	bool PrintInfo;

	Interrupt() : Function(nullptr), Tick(0), Name(""), PrintInfo() {}

	Interrupt(std::function<void()> function, uint32_t queueTick = 0, const char* name = nullptr, bool printInfo = false) : Function(function), Tick(queueTick), Name(name), PrintInfo(printInfo) {}
};

class InterruptQueue
{
	static constexpr size_t MaxDepth = 64;
	static std::array<Interrupt, MaxDepth> Queue;
	static volatile size_t InterruptsPending;

  public:
	static void AddInterrupt(const Interrupt& interrupt)
	{
		Queue[InterruptQueue::InterruptsPending] = interrupt;
		InterruptQueue::InterruptsPending = InterruptQueue::InterruptsPending + 1;
	}

	static void HandleQueue();
};

} // namespace PSR

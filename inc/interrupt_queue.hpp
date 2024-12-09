#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#include <array>
#include <functional>
#include <queue>

namespace PSR
{

class InterruptQueue
{
	static constexpr size_t MaxDepth = 64;
	static std::array<std::function<void()>, MaxDepth> Queue;
	static volatile size_t InterruptsPending;

  public:
	static void AddInterrupt(const std::function<void()>& callback)
	{
		Queue[InterruptQueue::InterruptsPending] = callback;
		InterruptQueue::InterruptsPending        = InterruptQueue::InterruptsPending + 1;
	}

	static void HandleQueue() __attribute__((section(".RamFunc")));
};

} // namespace PSR

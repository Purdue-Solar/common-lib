/*
 * interrupt_queue.hpp
 *
 *  Created on: Jun 2, 2024
 *      Author: aidan
 */

#ifndef INC_INTERRUPT_QUEUE_HPP_
#define INC_INTERRUPT_QUEUE_HPP_

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#include <functional>
#include <queue>
#include <cstdio>

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
	static constexpr size_t MaxDepth = 32;
	static std::array<Interrupt, MaxDepth> Queue;
	static volatile size_t InterruptsPending;

  public:
	static void AddInterrupt(const Interrupt& interrupt)
	{
		Queue[InterruptQueue::InterruptsPending++] = interrupt;
	}

	static void HandleQueue();
};

} // namespace PSR

#endif /* INC_INTERRUPT_QUEUE_HPP_ */

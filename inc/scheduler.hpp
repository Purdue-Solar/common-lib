/**
 * @file scheduler.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Contains logic for scheduling tasks on a fixed interval
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "interrupt_queue.hpp"
#include "timer_helpers.h"

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_rcc.h)

#include <array>
#include <bitset>
#include <functional>

namespace PSR
{

/**
 * @brief Scheduler class
 */
class Scheduler
{
  private:
	static constexpr size_t MaxTasks = 32;

	/// @brief The timer peripheral to use for the scheduler
	TIM_TypeDef* const tim;
	/// @brief The tasks to run
	/// @remark `nullptr` indicates an empty slot
	std::array<std::function<void()>, MaxTasks> tasks;
	/// @brief The intervals at which to run the tasks
	std::array<uint32_t, MaxTasks> intervals;
	/// @brief The offset from zero at which the tasks will run
	std::array<uint32_t, MaxTasks> startOffsets;
	/// @brief The counter value when the tasks will run next
	std::array<uint32_t, MaxTasks> nextUpdates;
	/// @brief The enabled tasks
	std::bitset<MaxTasks> enabledTasks;

	static constexpr size_t u = sizeof(tasks);

	/// @brief The internal counter used to track the scheduler
	uint32_t counter = 0;

	/// @brief The tick frequency of the scheduler
	const uint32_t frequency;
	/// @brief The number of ticks before the scheduler rolls over
	const uint32_t timerRollOver;

	/// @brief Whether the scheduler is initialized
	bool isInitialized = false;

	static constexpr uint32_t GetNextUpdate(uint32_t counter, uint32_t rollOver, uint32_t interval)
	{
		uint32_t nextUpdate = counter + interval;
		if (nextUpdate >= rollOver)
			nextUpdate -= rollOver;

		return nextUpdate;
	}

	static constexpr uint32_t GetFirstUpdate(uint32_t counter, uint32_t interval, uint32_t startOffset)
	{
		if (startOffset > counter)
			return startOffset;

		return counter + interval - (counter - startOffset) % interval;
	}

  public:
	/**
	 * @brief Construct a new Scheduler object
	 *
	 * @param tim The timer peripheral to use for the scheduler
	 * @param frequency The tick frequency of the scheduler
	 * @param rollOver The number of ticks before the scheduler rolls over
	 */
	Scheduler(TIM_TypeDef* tim, uint32_t frequency, uint32_t rollOver = std::numeric_limits<uint32_t>::max())
		: tim(tim), tasks(), intervals(), startOffsets(), nextUpdates(), enabledTasks(), frequency(frequency), timerRollOver(rollOver)
	{}

	constexpr size_t size() const
	{
		return MaxTasks;
	}

	uint32_t GetFrequency() const
	{
		return frequency;
	}

	uint32_t GetRollOverValue() const
	{
		return timerRollOver;
	}

	uint32_t GetCounter() const
	{
		return counter;
	}

	/**
	 * @brief Initialize the scheduler
	 * @remark This function must be called before adding tasks
	 *
	 * @return `bool` Whether the scheduler was initialized successfully, or is already initialized
	 */
	bool Init()
	{
		if (isInitialized)
			return true;

		if (tim == nullptr || frequency == 0 || frequency > HAL_RCC_GetSysClockFreq())
			return false;

		SetTimerFrequency(tim, frequency, 1);
		tim->DIER |= TIM_DIER_UIE;
		tim->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;

		tasks.fill(nullptr);
		intervals.fill(0);
		nextUpdates.fill(0);
		enabledTasks.reset();

		isInitialized = true;

		return true;
	}

	/**
	 * @brief Update the scheduler, adding tasks to the interrupt queue when they are due
	 * @remark This function should be called in the timer interrupt
	 */
	void Update()
	{
		if (!isInitialized)
			return;

		if (++counter >= timerRollOver)
			counter = 0;

		for (size_t i = 0; i < MaxTasks; i++)
		{
			if (tasks[i] == nullptr || !enabledTasks[i])
				continue;

			if (counter >= nextUpdates[i])
			{
				InterruptQueue::AddInterrupt(tasks[i]);
				if (intervals[i] == 0)
				{
					RemoveTask(i);
				}
				else
				{
					nextUpdates[i] = GetNextUpdate(counter, timerRollOver, intervals[i]);
				}
			}
		}
	}

	/**
	 * @brief Add a task to the scheduler
	 *
	 * @param task The function to call when the task is due
	 * @param interval The interval in ticks at which to run the task. Zero indicates a one-shot task
	 * @param startOffset The offset from zero at which the task will start to run
	 * @param enabled Whether the task is enabled
	 * @return `size_t` The index of the task in the scheduler, returns `std::numeric_limits<size_t>::max()` if the task could not be added
	 */
	size_t AddTask(const std::function<void()>& task, uint32_t interval, uint32_t startOffset = 0, bool enabled = true)
	{
		if (startOffset >= timerRollOver || interval >= timerRollOver)
			return std::numeric_limits<size_t>::max();

		for (size_t i = 0; i < MaxTasks; i++)
		{
			if (tasks[i] == nullptr)
			{

				tasks[i]        = task;
				intervals[i]    = interval;
				nextUpdates[i]  = GetFirstUpdate(counter, interval, startOffset);
				enabledTasks[i] = enabled;
				return i;
			}
		}

		return std::numeric_limits<size_t>::max();
	}

	/**
	 * @brief Removes a task from the scheduler
	 *
	 * @param index The index of the task to remove
	 * @return `bool` Whether the task was removed successfully
	 */
	bool RemoveTask(size_t index)
	{
		if (index >= MaxTasks)
			return false;

		tasks[index]        = nullptr;
		intervals[index]    = 0;
		nextUpdates[index]  = 0;
		enabledTasks[index] = false;

		return true;
	}

	/**
	 * @brief Enable a task
	 * @remark Does nothing if the task is already enabled, or an empty slot
	 * @param index The index of the task to enable
	 */
	void EnableTask(size_t index)
	{
		if (index >= MaxTasks)
			return;

		enabledTasks[index] = true;
	}

	/**
	 * @brief Disable a task
	 * @remark Does nothing if the task is already disabled, or an empty slot
	 * @param index The index of the task to disable
	 */
	void DisableTask(size_t index)
	{
		if (index >= MaxTasks)
			return;

		enabledTasks[index] = false;
	}

	/**
	 * @brief Get whether a task is enabled
	 *
	 * @param index The index of the task
	 * @return `bool` Whether the task is enabled, returns false if the slot is empty
	 */
	bool GetTaskEnabled(size_t index) const
	{
		if (index >= MaxTasks)
			return false;

		return enabledTasks[index];
	}

	/**
	 * @brief Set the interval of a task
	 * @remark Does nothing if the task is an empty slot
	 * @param index The index of the task
	 * @param interval The new interval in ticks
	 */
	void SetInterval(size_t index, uint32_t interval)
	{
		if (index >= MaxTasks)
			return;

		intervals[index] = interval;
	}

	/**
	 * @brief Get the interval of a task
	 * @remark Returns 0 if the task is an empty slot
	 * @param index The index of the task
	 * @return `uint32_t` The interval in ticks
	 */
	uint32_t GetInterval(size_t index) const
	{
		if (index >= MaxTasks)
			return 0;

		return intervals[index];
	}
};

} // namespace PSR
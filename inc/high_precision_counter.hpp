#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_rcc.h)

#include <array>
#include <cstdint>
#include <functional>

namespace PSR
{

class HighPrecisionCounter
{
  private:
	TIM_TypeDef* const tim;
	const uint32_t timerPrecision;
	uint64_t upperCount   = 0;
	uint64_t lastSyncTime = 0; ///< @brief the last time the counter was synced with an external source

	struct DelayedCallback
	{
		uint64_t DelayUntil;
		std::function<void()> Callback;

		DelayedCallback()
			: DelayUntil(0), Callback(nullptr)
		{}
	};

	static constexpr size_t MaxCallbacks = 32;
	std::array<DelayedCallback, MaxCallbacks> delayedCallbacks;

	/// @brief The highest index of a delayed callback
	size_t highestCallbackIndex = 0;

	bool isInitialized = false;

	void HandleDelayCallbacks();
	void ClearCallbacks();

  public:
	static constexpr uint32_t MillesecondsToMicroseconds = 1000;

	/**
	 * @brief Construct a new High Precision Counter object
	 *
	 * @param tim The timer peripheral to use for the counter
	 * @param timerPrecision The number of microseconds before the counter rolls over
	 */
	HighPrecisionCounter(TIM_TypeDef* const tim, uint32_t timerPrecision)
		: tim(tim), timerPrecision(timerPrecision), delayedCallbacks()
	{}

	/**
	 * @brief Initialize the counter
	 *
	 * @return `bool` Whether the counter was initialized successfully, or is already initialized
	 */
	bool Init();

	/**
	 * @brief Update the counter
	 * @param statusRegister The timer status register when the interrupt was triggered
	 * @param suppressCallbacks Whether to suppress the delayed callbacks
	 * @remark This function should be called in the timer interrupt
	 */
	void Update(uint32_t statusRegister, bool suppressCallbacks = false) __attribute__((section(".RamFunc")));

	/**
	 * @brief Get the current count of the timer
	 *
	 * @return `uint64_t` The current count in microseconds
	 */
	uint64_t GetCount() const
	{
		return this->upperCount + this->tim->CNT;
	}

	/// @brief Get the current time in microseconds since the timer started
	/// @return uint64_t The current time in microseconds
	/// @remark This is an alias for GetCount()
	uint64_t GetTime() const { return GetCount(); }

	/**
	 * @brief Get the upper count of the timer
	 * 
	 * @return uint64_t The current upper counter in microseconds
	 */
	uint64_t GetUpperCount() const
	{
		return this->upperCount;
	}

	/**
	 * @brief Get the lower count from the timer
	 * 
	 * @return uint32_t The current lower counter in microseconds
	 */
	uint32_t GetLowerCount() const
	{
		return this->tim->CNT;
	}

	/**
	 * @brief Get the number of microseconds before the lower counter rolls over
	 * 
	 * @return uint32_t The timer precision in microseconds
	 */
	uint32_t GetPrecision() const
	{
		return this->timerPrecision;
	}

	/**
	 * @brief Get the timer peripheral used by this counter
	 * 
	 * @return TIM_TypeDef* The timer peripheral
	 */
	TIM_TypeDef* GetTimer() const
	{
		return this->tim;
	}

	/**
	 * @brief Get the last time that the counter was synchronized with an external source
	 * 
	 * @return uint64_t The last synchronization time in microseconds
	 */
	uint64_t GetLastSyncTime() const
	{
		return this->lastSyncTime;
	}

	/**
	 * @brief Reset the counter and clear all callbacks
	 */
	void Reset()
	{
		upperCount   = 0;
		lastSyncTime = 0;
		tim->CNT     = 0;

		ClearCallbacks();
	}

	/**
	 * @brief Delay for a number of microseconds
	 *
	 * @remark This function will block execution for the specified number of microseconds
	 *
	 * @param microseconds The number of microseconds to delay
	 */
	void Delay(int32_t microseconds)
	{
		if (microseconds < 0)
			return;

		uint64_t start = GetCount();
		uint64_t end   = start + microseconds;
		while (GetCount() < end) {
		}
	}

	/**
	 * @brief Add a callback to be called after a delay
	 *
	 * @remark The callback will be called in a non-interrupt context, after all other interrupts have been handled
	 *
	 * @param delay The delay in ms
	 * @param callback The callback to execute
	 * @return `bool` Whether the callback was added
	 */
	bool AddDelayedCallback(uint32_t delay, const std::function<void()>& callback);

	/// @brief Synchronize the counter with an external source
	/// @param exectedDelay The expected delay from the previous call to this function
	void Synchronize(uint32_t exectedDelay)
	{
		uint64_t currentTime = GetCount();
		uint64_t expectedTime = lastSyncTime + exectedDelay;

		int64_t delta = expectedTime - currentTime;

		upperCount += delta;
		lastSyncTime = GetCount();
	}
};

} // namespace PSR

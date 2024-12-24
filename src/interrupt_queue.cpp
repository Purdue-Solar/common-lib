#include "interrupt_queue.hpp"

#include <cstdio>

using namespace PSR;

std::array<std::function<void()>, InterruptQueue::MaxDepth> InterruptQueue::Queue;
volatile size_t InterruptQueue::InterruptsPending = 0;

bool InterruptQueue::AddInterrupt(const std::function<void()>& callback)
{
	if (InterruptQueue::InterruptsPending >= InterruptQueue::MaxDepth)
		return false;

	// Disable interrupts while modifying the queue
	uint32_t primask_bit = __get_PRIMASK();
	__disable_irq();

	Queue[InterruptQueue::InterruptsPending] = callback;
	InterruptQueue::InterruptsPending        = InterruptQueue::InterruptsPending + 1;

	// Re-enable interrupts
	__set_PRIMASK(primask_bit);

	return true;
}

void InterruptQueue::HandleQueue()
{
	if (InterruptsPending > 0)
	{
		for (size_t i = 0; i < InterruptsPending; i++)
		{
			const std::function<void()>& interrupt = InterruptQueue::Queue[i];
			if (interrupt)
			{
				interrupt();

				uint32_t primask = __get_PRIMASK();
				__disable_irq();
				InterruptQueue::Queue[i] = nullptr;
				__set_PRIMASK(primask);
			}
		}

		InterruptsPending = 0;
	}
}

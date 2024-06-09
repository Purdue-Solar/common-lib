#include "interrupt_queue.hpp"

using namespace PSR;

std::array<Interrupt, InterruptQueue::MaxDepth> InterruptQueue::Queue;
volatile size_t InterruptQueue::InterruptsPending = 0;

void InterruptQueue::HandleQueue()
{
	if (InterruptsPending > 0)
	{
		for (size_t i = 0; i < InterruptsPending; i++)
		{
			Interrupt& interrupt = InterruptQueue::Queue[i];
			if (interrupt.Function) {
				interrupt.Function();

#if defined(PRINT_INTERRUPT_DELAY) && PRINT_INTERRUPT_DELAY != 0
				if (interrupt.PrintInfo)
					printf("%s Delay: %ld\n", interrupt.Name, HAL_GetTick() - interrupt.Tick);
#endif
			}
		}

		InterruptsPending = 0;
	}
}

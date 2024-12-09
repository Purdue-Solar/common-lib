#include "interrupt_queue.hpp"

#include <cstdio>

using namespace PSR;

std::array<std::function<void()>, InterruptQueue::MaxDepth> InterruptQueue::Queue;
volatile size_t InterruptQueue::InterruptsPending = 0;

void InterruptQueue::HandleQueue()
{
	if (InterruptsPending > 0) {
		for (size_t i = 0; i < InterruptsPending; i++) {
			const std::function<void()>& interrupt = InterruptQueue::Queue[i];
			if (interrupt) interrupt();
		}

		InterruptsPending = 0;
	}
}

# Common Lib

Includes some frequently used pieces of code for STM32 microcontrollers.

- bit_operations.h - Useful extended bitwise operations (set, extract, rotate, reverse endianness, etc.)
- delay.h - Simplified microsecond delay functions
- gpio_pin.hpp - Wrapper class for easily manipulating GPIO pins
- high_precision_counter.hpp - Microsecond counter for measuring time over long periods
- interrupt_queue.hpp - Queue to allow generating callbacks during interrupts that get run in a non-interrupt context
- scheduler.hpp - Class to run tasks at regular intervals
- stm32_includer.h - Simplifies generation of STM32 specific includes
- timer_helpers.h - Helper functions for manipulating and get information from timers

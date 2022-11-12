#ifdef BOARD_STM32F

#if BOARD_STM32F == 0
#include "stm32/f0_common.h"
#elif BOARD_STM32F == 1
#include "stm32/f1_common.h"
#elif BOARD_STM32F == 2
#include "stm32/f2_common.h"
#elif BOARD_STM32F == 3
#include "stm32/f3_common.h"
#elif BOARD_STM32F == 4
#include "stm32/f4_common.h"
#else
#error STM32 board type not recognized
#endif

#endif
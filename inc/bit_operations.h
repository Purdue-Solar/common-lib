/**
 * @file bit_operations.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief
 * @version 0.1
 * @date 2023-01-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __BIT_OPERATIONS_H
#define __BIT_OPERATIONS_H

#include <stdint.h>

namespace PSR
{

/**
 * @brief Extract a single bit at an index from a value
 *
 * @tparam int_type the integer type of the value
 * @param value the value to extract bits from
 * @param bit the bit index to extract
 * @return int the extracted bit (1 or 0)
 */
template <class int_type>
int bitExtract(int_type value, int bit)
{
	return (int)((value >> bit) & 1);
}

/**
 * @brief Extract a range of bits from a value
 *
 * @tparam int_type the integer type of the value
 * @param value the value to extract bits from
 * @param index the start index of the bits to extract
 * @param count the number of bits to extract
 * @return int_type the extracted bits
 */
template <class int_type>
int_type bitExtract(int_type value, int index, int count)
{
	return (value >> index) & (((int_type)1 << count) - 1);
}

} // namespace PSR

#endif // End of include guard for bit_opeartions.h
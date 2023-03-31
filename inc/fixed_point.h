/**
 * @file fixed_point.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Implements Fixed Basic Point Arithmetic
 * @version 0.1
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __FIXED_POINT_H
#define __FIXED_POINT_H

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace __DISCARD
{
template <typename>
struct next_size;

template <class T>
using next_size_t = typename next_size<T>::type;

template <class T>
struct tag
{
	using type = T;
};

template <>
struct next_size<uint8_t> : tag<uint16_t>
{};
template <>
struct next_size<uint16_t> : tag<uint32_t>
{};
template <>
struct next_size<uint32_t> : tag<uint64_t>
{};
template <>
struct next_size<uint64_t> : tag<void>
{};

template <>
struct next_size<int8_t> : tag<uint16_t>
{};
template <>
struct next_size<int16_t> : tag<uint32_t>
{};
template <>
struct next_size<int32_t> : tag<uint64_t>
{};
template <>
struct next_size<int64_t> : tag<void>
{};
}; // namespace __DISCARD

namespace PSR
{

/**
 * @brief Implements basic binary fixed point arithmetic
 *
 * @tparam fracBits the number of fractional bits
 * @tparam Tint the underlying integer type
 */
template <size_t fracBits = 16, typename Tint = uint32_t>
struct FixedPoint
{
  private:
	Tint _value;

	static constexpr Tint _fracMask = (1ULL << fracBits) - 1;
	static constexpr Tint _scaling = (1ULL << fracBits);

	static constexpr size_t BITS =
		sizeof(FixedPoint<fracBits, Tint>) * CHAR_BIT;
	static constexpr size_t HALF_BITS = BITS / 2;

	static constexpr Tint HALF_BITS_MASK = (1ULL << HALF_BITS) - 1;

	using sTint = typename std::make_signed<Tint>::type;

  public:
	/**
	 * @brief Initializes the fixed point value to its default value, 0.
	 */
	constexpr FixedPoint()
		: _value() {}

	/**
	 * @brief Initializes the fixed point value with its integer part
	 *
	 * @param intValue the integer value to set
	 */
	constexpr FixedPoint(Tint intValue)
		: _value(intValue << fracBits) {}

	/**
	 * @brief Initializes the fixed point value with its integer part
	 *
	 * @param intValue the integer value to set
	 */
	constexpr FixedPoint(sTint intValue)
		: _value(intValue << fracBits) {}

	/**
	 * @brief Initialize the fixed point value with an integer and fractional
	 * part
	 *
	 * @param integerPart the integral part of the fixed point value
	 * @param fractionalPart the fractional part of the fixed point value
	 */
	constexpr FixedPoint(Tint integerPart, Tint fractionalPart)
		: _value((integerPart << fracBits) | (fractionalPart & _fracMask)) {}

	/**
	 * @brief Converts a float value to a fixed point representation.
	 *
	 * @param value the float value to convert.
	 */
	constexpr FixedPoint(float value)
		: _value((Tint)(value * _scaling)) {}

	/**
	 * @brief Converts a double value to a fixed point representation.
	 *
	 * @param value the double value to convert.
	 */
	constexpr FixedPoint(double value)
		: _value((Tint)(value * _scaling)) {}

	/**
	 * @brief Converts a long double value to a fixed point representation.
	 *
	 * @param value the long double value to convert.
	 */
	constexpr FixedPoint(long double value)
		: _value((Tint)(value * _scaling)) {}

	static constexpr FixedPoint<fracBits, Tint> FromRaw(Tint value)
	{
		FixedPoint<fracBits, Tint> v;
		v._value = value;
		return v;
	}

	static constexpr FixedPoint<fracBits, Tint> Zero()
	{
		return FixedPoint<fracBits, Tint>(0, 0);
	}
	static constexpr FixedPoint<fracBits, Tint> One()
	{
		return FixedPoint<fracBits, Tint>(1, 0);
	}
	static constexpr FixedPoint<fracBits, Tint> OneHalf()
	{
		return FixedPoint<fracBits, Tint>(0, (1ULL << (fracBits - 1)));
	}
	static constexpr FixedPoint<fracBits, Tint> MinusOne()
	{
		return FixedPoint<fracBits, Tint>(-1, 0);
	}
	static constexpr FixedPoint<fracBits, Tint> MinusOneHalf()
	{
		return FixedPoint<fracBits, Tint>(-1, (1ULL << (fracBits - 1)));
	}

	template <size_t newFracBits, typename newIntType = Tint>
	constexpr FixedPoint<newFracBits, newIntType> rescale() const
	{
		newIntType intPart = _value >> fracBits;
		newIntType fracPart = _value & _fracMask;

		return FixedPoint<newFracBits, newIntType>(intPart, fracPart);
	}

	friend constexpr FixedPoint<fracBits, Tint> operator+(
		const FixedPoint<fracBits, Tint>& value)
	{
		return value;
	}

	friend constexpr FixedPoint<fracBits, Tint> operator-(
		const FixedPoint<fracBits, Tint>& value)
	{
		return FromRaw((Tint) - (sTint)value);
	}

	friend constexpr FixedPoint<fracBits, Tint> operator+(
		const FixedPoint<fracBits, Tint>& left,
		const FixedPoint<fracBits, Tint>& right)
	{
		return FromRaw(left + right);
	}

	constexpr FixedPoint<fracBits, Tint> operator+=(
		const FixedPoint<fracBits, Tint>& right)
	{
		return *this = *this + right;
	}

	friend constexpr FixedPoint<fracBits, Tint> operator-(
		const FixedPoint<fracBits, Tint>& left,
		const FixedPoint<fracBits, Tint>& right)
	{
		return FromRaw(left - right);
	}

	constexpr FixedPoint<fracBits, Tint> operator-=(
		const FixedPoint<fracBits, Tint>& right)
	{
		return *this = *this - right;
	}

	friend constexpr FixedPoint<fracBits, Tint> operator*(
		const FixedPoint<fracBits, Tint>& left,
		const FixedPoint<fracBits, Tint>& right)
	{
		// Check if the next largest integer type exists and use built-in multiply
		if (!std::is_void<__DISCARD::next_size<Tint>>::value) {
			typename __DISCARD::next_size<Tint>::type l = left._value;
			typename __DISCARD::next_size<Tint>::type r = right._value;
			typename __DISCARD::next_size<Tint>::type v = l * r;
			return FromRaw((Tint)(v >> fracBits));
		}

		// Tint is already the maximum integer type, must multiply by hand
		Tint l = left._value;
		Tint r = right._value;

		Tint u0 = l >> HALF_BITS;
		Tint v0 = r >> HALF_BITS;
		Tint u1 = l & HALF_BITS_MASK;
		Tint v1 = r & HALF_BITS_MASK;

		Tint t = u1 * v1;
		Tint w3 = t & HALF_BITS_MASK;
		Tint k = t >> HALF_BITS;

		t = u0 * v1 + k;
		Tint w2 = t & HALF_BITS_MASK;
		Tint w1 = t >> HALF_BITS;

		t = u1 * v0 + w2;
		k = t >> HALF_BITS;

		Tint hi = u0 * v0 + w1 + k;
		Tint lo = l * r;

		return FromRaw((hi << (BITS - fracBits)) | lo >> fracBits);
	}

	constexpr FixedPoint<fracBits, Tint> operator*=(
		const FixedPoint<fracBits, Tint>& right)
	{
		return *this = *this * right;
	}

	template <size_t otherFracBits>
	friend constexpr FixedPoint<fracBits, Tint> operator/(FixedPoint<fracBits, Tint> left, FixedPoint<otherFracBits, Tint> right)
	{
		Tint lv = left._value;
		Tint rv = right._value;

		Tint q = lv / rv;
		int bitDiff = fracBits - otherFracBits;
		if (bitDiff < 0)
			q >>= -bitDiff;
		else
			q <<= bitDiff;

		return FromRaw(q);
	}

	friend constexpr bool operator==(const FixedPoint<fracBits, Tint>& left,
	                                 const FixedPoint<fracBits, Tint>& right)
	{
		return left._value == right._value;
	}

	friend constexpr bool operator!=(const FixedPoint<fracBits, Tint>& left,
	                                 const FixedPoint<fracBits, Tint>& right)
	{
		return left._value != right._value;
	}

	friend constexpr bool operator<(const FixedPoint<fracBits, Tint>& left,
	                                const FixedPoint<fracBits, Tint>& right)
	{
		return (sTint)left._value < (sTint)right._value;
	}

	friend constexpr bool operator<=(const FixedPoint<fracBits, Tint>& left,
	                                 const FixedPoint<fracBits, Tint>& right)
	{
		return (sTint)left._value <= (sTint)right._value;
	}

	friend constexpr bool operator>(const FixedPoint<fracBits, Tint>& left,
	                                const FixedPoint<fracBits, Tint>& right)
	{
		return (sTint)left._value > (sTint)right._value;
	}

	friend constexpr bool operator>=(const FixedPoint<fracBits, Tint>& left,
	                                 const FixedPoint<fracBits, Tint>& right)
	{
		return (sTint)left._value >= (sTint)right._value;
	}

	explicit operator sTint() { return ((sTint)_value) >> fracBits; }
	explicit operator Tint() { return _value >> fracBits; }
	explicit operator float() { return (float)_value / _scaling; }
	explicit operator double() { return (double)_value / _scaling; }
	explicit operator long double() { return (long double)_value / _scaling; }
};

using FP8_8 = FixedPoint<8, uint16_t>;
using FP16_16 = FixedPoint<16, uint32_t>;
using FP32_32 = FixedPoint<32, uint64_t>;

constexpr FP16_16 operator""_fp(long double value)
{
	return FP16_16(value);
}

constexpr FP8_8 operator""_fp8_8(long double value)
{
	return FP8_8(value);
}

constexpr FP16_16 operator""_fp16_16(long double value)
{
	return FP16_16(value);
}

constexpr FP32_32 operator""_fp32_32(long double value)
{
	return FP32_32(value);
}

} // namespace PSR

#endif
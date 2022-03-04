#pragma once

#include <stdint.h>
#include <string>
#include <sys/types.h>

/// uint64_least will be evaluated to uint64_t, if dist is smaller than 64, else it will be void and cause an error.
template< int dist >
using uint64_least = typename std::conditional< dist <= 64, uint64_t, void >::type;
/// uint32_least will be evaluated to the smallest uint type with at least 32 bits, which can hold dist bits.
template< int dist >
using uint32_least = typename std::conditional< dist <= 32, uint32_t, uint64_least< dist > >::type;
/// uint16_least will be evaluated to the smallest uint type with at least 16 bits, which can hold dist bits.
template< int dist >
using uint16_least = typename std::conditional< dist <= 16, uint16_t, uint32_least< dist > >::type;
/// uint_least will be evaluated to the smallest uint type, which can hold dist bits.
template< int dist >
using uint_least = typename std::conditional< dist <= 8, uint8_t, uint16_least< dist > >::type;

/// A HexaDecimal holds the hexadecimal textual and numerical representation of a number.
template< uint BITS >
class HexaDecimal {
	// we want a 1to1 conversion of an uint
	static_assert(BITS == 8 || BITS == 16 || BITS == 32 || BITS == 64, "number of bits invalid!");

	static constexpr char hexLetters[] = "0123456789ABCDEF";	///< number to hex map

	std::string		   text;	///< hexadecimal textual representation of this number.
	uint_least< BITS > num;		///< the plain number

  public:
	/*!
	 * \brief HexaDecimal conversion from a std::string.
	 * \param s Sets the textual representation.
	 */
	constexpr HexaDecimal(const std::string &s)
		: text(s),
		  num(0) {}

	/*!
	 * \brief HexaDecimal conversion from a number.
	 * \param n Sets the number.
	 */
	constexpr HexaDecimal(uint_least< BITS > n)
		: text(),
		  num(n) {}

	/// Conversion to the hexadecimal textual representation of this number. Initializing it, if it wasn't in before.
	operator std::string() {
		if (text.empty() && num != 0) {
			text.back() = '\0';
			auto tmp	= num;
			for (auto byte = text.rbegin() + 1; byte != text.rend(); ++byte) {
				*byte = hexLetters[tmp & 0xF];
				tmp >> 4;
			}
		}
		return text;
	}
	/// Conversion to the plain number. Initializing it, if it wasn't in before.
	constexpr operator uint_least< BITS >() {
		if (num == 0 && !text.empty()) {
			num = strtoul(text.data(), nullptr, 16);
		}
		return num;
	}
};
/// A deduction guide to get number of BITS from a literal string.
template< auto N >
HexaDecimal(const char (&)[N]) -> HexaDecimal< (N - 1) * 4 >;

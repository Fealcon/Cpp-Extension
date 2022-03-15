#pragma once

#include <array>
#include <string>
#include <sys/types.h>

/// CharArray is a specialization of std::array<char>, which adds compile time conversion from/to literal strings and conversion from/to std::strings. It can be used as template argument wrapper for literal strings.
template< ulong N >
class CharArray : public std::array< char, N > {
  public:
	using array = std::array< char, N >;
	using array::array;

	/// Conversion from a literal string at compile time..
	constexpr CharArray(const char *txt) {
		for (auto &c : *this) {
			c = *txt;
			++txt;
		}
	}
	/// Conversion to a char pointer,
	constexpr operator char *() {
		return this->data();
	}

	/// Conversion from a std::string
	CharArray(const std::string &s)
		: CharArray(s.data()) {}
	/// Conversion to a std::string
	operator std::string() {
		return this->data();
	}
};
/// A deduction guide to get array size from a literal string.
template< auto N >
CharArray(const char (&)[N]) -> CharArray< N >;

/// StaticString is a wrapper to a string literal, which can be used as param in constexpr and consteval functions to pass string literals with length.
class StaticString {
	/// CalcLength computes the length of a string literal at compile time to set #length.
	static consteval size_t CalcLength(const char str[]) {
		if (str[0] != '\0') {
			return CalcLength(str + 1) + 1;
		} else {
			return 0;
		}
	}

  public:
	const char *const str; ///< A pointer to the string literal, which was used to construct this class.
	const size_t length; ///< The length of the string literal.

	/// Construct a StaticString around the string literal \a string.
	consteval StaticString(const char *string)
		: str(string),
		  length(CalcLength(string)) {}

	/// Convert back to a char pointer to enable usage like a usual string literal.
	constexpr operator const char *() {
		return str;
	}

	/// Construct a StaticString, which points to the data of a CharArray.
	template< uint N >
	consteval StaticString(const CharArray< N > &fix)
		: str(fix),
		  length(N) {}

	/// Convert to a CharArray.
	template< uint N >
	constexpr operator CharArray< N >() {
		return CharArray< N >(str);
	}
};

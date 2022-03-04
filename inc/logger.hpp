#pragma once

#include "textuals.hpp"

#include <array>
#include <fstream>
#include <iostream>
#include <stdio.h>

/// Class Log implements default logging channels and a facility for more custom channels. The logs will be printed to cerr or cout and to a file, if #logfile is set.
class Log {
  public:
	/// Class Name creates a compile time string from a string literal, which has always the same length, filled with whitespace.
	struct Name {
		char str[20];	 ///< str allocates the memory for the string.

		/// The constructor takes a string literal, stores it in #str and appends ':' and whitespaces until #str is filled.
		consteval Name(const char *s) {
			char *const str_end = str + sizeof(str) - 1;

			// copy s into str
			const char *pt	   = s;
			char *		str_pt = str;
			while (true) {
				if (*pt == '\0') {
					break;
				}
				if (str_pt == str_end - 1) {
					break;
				}
				*(str_pt++) = *(pt++);
			}
			// add ':'
			*(str_pt++) = ':';
			// fill the rest with whitespaces
			while (true) {
				if (str_pt == str_end) {
					*str_pt = '\0';
					break;
				}
				*str_pt = ' ';
				++str_pt;
			}
		}

		/// The conversion to a const char pointer returns #str.
		constexpr operator const char *() {
			return str;
		}
	};

  private:
	/// Writer is a conveniance struct to simultaneously write to cout/cerr and a file.
	template< std::ostream &out >
	struct Writer {
		/// The operator <<< will put arg into cout/cerr and the #fileStream.
		template< class C >
		Writer &operator<<(C arg) {
			out << arg;
			fileStream << arg;
			return *this;
		}
	};

	static std::ofstream fileStream;	///< The file stream to write into #logfile, if it is not empty.

	static void move_logs();	///< Will rename and compress current logfiles to avoid using too much space.

	/// Log the last arg, close #fileStream and #move_logs.
	template< std::ostream &out, class A >
	static constexpr void log_arg(A arg) {
		Writer< out >() << arg
						<< '\n';
		fileStream.close();
		move_logs();
	}

	/// Log an arg and keep on until we reach the last arg.
	template< std::ostream &out, class A, class... C >
	static constexpr void log_arg(A arg, C... args) {
		Writer< out >() << arg;
		log_arg< out >(args...);
	}

	/// Open #fileStream and write the Log.
	template< Name NAME, std::ostream &out, class... C >
	static constexpr void log(C... args) {
		fileStream.open(logfile, std::ios::out | std::ios::app);
		Writer< out >() << NAME.str;
		log_arg< out >(args...);
	}

  public:
	static const char *logfile;	   ///< A file, where all logs are saved in, when set.

	static bool trace;		 ///< Enable/Disable #Trace logs.
	static bool warnings;	 ///< Enable/Disable #Warn logs.
	static bool errors;		 ///< Enable/Disable #Error logs.

	/// A factory to create logs on cout and prepend them with \a NAME.
	template< Name NAME, class... C >
	static constexpr void default_log(C... args) {
		log< NAME, std::cout >(args...);
	}

	/// A factory to create logs on cerr and prepend them with \a NAME.
	template< Name NAME, class... C >
	static constexpr void err_log(C... args) {
		log< NAME, std::cerr >(args...);
	}

	/*!
	 * \brief Trace logs to cout and prepends "trace", when #trace is true.
	 * \details This log should be used to trace the program execution for debugging and should not be active in production.
	 */
	template< class... C >
	static constexpr void Trace(C... args) {
		if (trace) {
			default_log< "trace" >(args...);
		}
	}

	/*!
	 * \brief Warn logs to cerr and prepends "warning", when #warnings is true.
	 * \details This log should be used to log all unexpected behavior, that may still allow the program to keep working correctly.
	 */
	template< class... C >
	static constexpr void Warn(C... args) {
		if (warnings) {
			err_log< "warning" >(args...);
		}
	}

	/*!
	 * \brief Error logs to cerr and prepends "error", when #errors is true.
	 * \details This log should be used for all unexpected behavior, that blocks the program from working correctly.
	 */
	template< class... C >
	static constexpr void Error(C... args) {
		if (errors) {
			err_log< "error" >(args...);
		}
	}
};

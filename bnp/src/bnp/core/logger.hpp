#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <cstdarg>  // For variadic argument handling

namespace bnp {

	namespace Log {

		// Internal helper function to format messages
		template<typename... Args>
		void log(const std::string& level, const char* format, Args&&... args) {
			std::ostringstream message;
			message << "[" << level << "] ";

			// Create a formatted string using snprintf or stringstream
			char buffer[1024];  // Buffer for formatted message
			std::snprintf(buffer, sizeof(buffer), format, std::forward<Args>(args)...);
			message << buffer;

			std::cout << message.str() << std::endl;
		}

		// Public API for logging at the "info" level
		template<typename... Args>
		void info(const char* format, Args&&... args) {
			log("INFO", format, std::forward<Args>(args)...);
		}

		// Public API for logging at the "warning" level
		template<typename... Args>
		void warning(const char* format, Args&&... args) {
			log("WARNING", format, std::forward<Args>(args)...);
		}

		// Public API for logging at the "error" level
		template<typename... Args>
		void error(const char* format, Args&&... args) {
			log("ERROR", format, std::forward<Args>(args)...);
		}

	}

}

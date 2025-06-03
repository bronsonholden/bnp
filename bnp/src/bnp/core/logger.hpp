#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <cstdarg>  // For variadic argument handling
#include <cwchar>   // For swprintf

namespace bnp {

namespace Log {

// Internal helper function to format messages (narrow string)
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

// Internal helper function to format messages (wide string)
template<typename... Args>
void log_wide(const std::wstring& level, const wchar_t* format, Args&&... args) {
	std::wostringstream message;
	message << L"[" << level << L"] ";

	// Create a formatted string using swprintf
	wchar_t buffer[1024];  // Buffer for formatted wide message
	std::swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, std::forward<Args>(args)...);
	message << buffer;

	std::wcout << message.str() << std::endl;
}

// Public API for logging at the "info" level (narrow string)
template<typename... Args>
void info(const char* format, Args&&... args) {
	log("INFO", format, std::forward<Args>(args)...);
}

// Public API for logging at the "warning" level (narrow string)
template<typename... Args>
void warning(const char* format, Args&&... args) {
	log("WARNING", format, std::forward<Args>(args)...);
}

// Public API for logging at the "error" level (narrow string)
template<typename... Args>
void error(const char* format, Args&&... args) {
	log("ERROR", format, std::forward<Args>(args)...);
}

// Public API for logging at the "info" level (wide string)
template<typename... Args>
void info_wide(const wchar_t* format, Args&&... args) {
	log_wide(L"INFO", format, std::forward<Args>(args)...);
}

// Public API for logging at the "warning" level (wide string)
template<typename... Args>
void warning_wide(const wchar_t* format, Args&&... args) {
	log_wide(L"WARNING", format, std::forward<Args>(args)...);
}

// Public API for logging at the "error" level (wide string)
template<typename... Args>
void error_wide(const wchar_t* format, Args&&... args) {
	log_wide(L"ERROR", format, std::forward<Args>(args)...);
}

}

}

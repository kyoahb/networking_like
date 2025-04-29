#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include "LogScope.h"

class Log {
	// Static class used for logging info / warnings / errors to the console.

public:
	static void init(); // Initialise the logger

	// Context-aware logging methods
	static void trace(const std::string& message);
	static void info(const std::string& message);
	static void warn(const std::string& message);
	static void error(const std::string& message);
	static void critical(const std::string& message);

	// Assertion helper
	static void asserts(bool condition, const std::string& message); // Log an error + quit if condition is false

private:
	static bool initialised; // Whether the logger has been initialised

	static std::string folder_name; // "logs"
	static std::string generate_filename(); // Generate log filename from date and time
	static void ensure_folder_exists(const std::string& folder); // Create folder if it doesn't exist
};
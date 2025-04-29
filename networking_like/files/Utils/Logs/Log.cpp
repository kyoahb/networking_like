#include "Log.h"

bool Log::initialised = false;
std::string Log::folder_name = "logs";

void Log::init() {
    // Create the logs folder if it doesn't exist
    ensure_folder_exists(Log::folder_name);

    // Generate the log filename
    std::string filename = generate_filename();

    // Create console and file sinks
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);

    // Create a logger with both sinks
    spdlog::sinks_init_list sink_list = { console_sink, file_sink };
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sink_list.begin(), sink_list.end());
    spdlog::set_default_logger(logger);

    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%-7l%$] %v"); // Set log pattern to include date and time

    spdlog::set_level(spdlog::level::trace); // Set global log level to trace
    spdlog::flush_on(spdlog::level::trace);  // Flush log on each info level message

    spdlog::info("Finished loading logger.");
    Log::initialised = true;
}

// Rest of Log.cpp methods remain unchanged
void Log::trace(const std::string& message) {
    if (!Log::initialised) Log::init();
    spdlog::trace(LogScope::add_context_prefix(message, LogScope::get_current_context()));
}

void Log::info(const std::string& message) {
    if (!Log::initialised) Log::init();
    spdlog::info(LogScope::add_context_prefix(message, LogScope::get_current_context()));
}

void Log::warn(const std::string& message) {
    if (!Log::initialised) Log::init();
    spdlog::warn(LogScope::add_context_prefix(message, LogScope::get_current_context()));
}

void Log::error(const std::string& message) {
    if (!Log::initialised) Log::init();
    spdlog::error(LogScope::add_context_prefix(message, LogScope::get_current_context()));
}

void Log::critical(const std::string& message) {
    if (!Log::initialised) Log::init();
    spdlog::critical(LogScope::add_context_prefix(message, LogScope::get_current_context()));
}

void Log::asserts(bool condition, const std::string& message) {
    if (!condition) {
        critical("ASSERTION FAILED: " + message);
        throw std::runtime_error(message);
    }
}

std::string Log::generate_filename() {
    // Generates a filename from date and time
    // Format: logs/DD-MM-YYYY_HH:MM:SS.log

    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    // Format the time
    std::tm time_info;
    localtime_s(&time_info, &in_time_t); // Use localtime_s instead of localtime

    std::stringstream ss;
    ss << std::put_time(&time_info, "%d-%m-%Y_%H-%M-%S");
    std::string time = ss.str();

    // Generate the filename
    std::string filename = Log::folder_name + "/" + time + ".log";
    return filename;
}

void Log::ensure_folder_exists(const std::string& folder) {
    // Create the logs folder if it doesn't exist
    if (!std::filesystem::exists(folder)) {
        std::filesystem::create_directory(folder);
    }
}
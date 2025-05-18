#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

enum class LogContext {
    GENERAL,
    SERVER,
    SERVER_PROTOCOL,
    CLIENT,
    CLIENT_PROTOCOL,
    SHARED_NETWORKING,
    TEST,
	SERVER_GROUP,
	CLIENT_GROUP
};

class LogScope {
    // A static class to manage logging context and formatting. Thread-local
private:
    friend class Log; // Allow Log class to access current context
    static thread_local std::string current_function;
    static thread_local LogContext current_context;
    std::string previous_function;
    LogContext previous_context;

public:
    LogScope(const std::string& function_name, LogContext context = LogContext::GENERAL);
    ~LogScope();

    static std::string format_fixed_width(const std::string& text, size_t width, bool right_align = false, bool truncate_left = true);
    static std::string add_context_prefix(const std::string& message, LogContext context);
    static std::string get_current_function();
    static LogContext get_current_context();
};

#define LOG_SCOPE LogScope __log_scope__(__FUNCTION__, LogContext::GENERAL);
#define LOG_SCOPE_SERVER LogScope __log_scope__(__FUNCTION__, LogContext::SERVER);
#define LOG_SCOPE_SERVER_PROTOCOL LogScope __log_scope__(__FUNCTION__, LogContext::SERVER_PROTOCOL);
#define LOG_SCOPE_SERVER_GROUP LogScope __log_scope__(__FUNCTION__, LogContext::SERVER_GROUP);
#define LOG_SCOPE_NET LogScope __log_scope__(__FUNCTION__, LogContext::SHARED_NETWORKING);
#define LOG_SCOPE_CLIENT LogScope __log_scope__(__FUNCTION__, LogContext::CLIENT);
#define LOG_SCOPE_CLIENT_PROTOCOL LogScope __log_scope__(__FUNCTION__, LogContext::CLIENT_PROTOCOL);
#define LOG_SCOPE_CLIENT_GROUP LogScope __log_scope__(__FUNCTION__, LogContext::CLIENT_GROUP);
#define LOG_SCOPE_TEST LogScope __log_scope__(__FUNCTION__, LogContext::GENERAL);
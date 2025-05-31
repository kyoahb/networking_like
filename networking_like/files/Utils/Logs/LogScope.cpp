#include "LogScope.h"

thread_local std::string LogScope::current_function = "";
thread_local LogContext LogScope::current_context = LogContext::GENERAL;

LogScope::LogScope(const std::string& function_name, LogContext context)
    : previous_function(current_function), previous_context(current_context) {
    // Store current function and context
    current_function = function_name;
    current_context = context;
}

LogScope::~LogScope() {
    // Restore previous function name and context
    current_function = previous_function;
    current_context = previous_context;
}

std::string LogScope::format_fixed_width(const std::string& text, size_t width, bool right_align, bool truncate_left) {
    if (text.length() == width) {
        // Text is already the correct length
        return text;
    }
    else if (text.length() > width) {
        // Text needs truncating
        if (truncate_left) {
            // Truncate from left side, keeping the right portion (more distinctive for thread IDs)
            return "." + text.substr(text.length() - (width - 1));
        }
        else {
            // Truncate from right side, keeping the left portion (more meaningful for function names)
            return text.substr(0, width - 1) + ".";
        }
    }
    else {
        // Text needs padding
        std::string padding(width - text.length(), ' ');
        if (right_align) {
            // Right-align by adding padding to the left
            return padding + text;
        }
        else {
            // Left-align by adding padding to the right
            return text + padding;
        }
    }
}

std::string LogScope::get_current_function() {
    return current_function;
}

LogContext LogScope::get_current_context() {
    return current_context;
}

std::string LogScope::add_context_prefix(const std::string& message, LogContext context) {
    // Get the thread ID
    std::stringstream thread_ss;
    thread_ss << std::this_thread::get_id();
    std::string thread_id = "[" + format_fixed_width(thread_ss.str(), 5, true) + "] ";

    std::string context_str;
    switch (context) {
    case LogContext::SERVER:
        context_str = "\033[1;32mSERVER\033[0m | " + thread_id;
        break;
    case LogContext::CLIENT:
        context_str = "\033[1;36mCLIENT\033[0m | " + thread_id;
        break;
	case LogContext::SHARED_NETWORKING:
		context_str = "\033[1;34mSHARED\033[0m | " + thread_id;
		break;
	case LogContext::GENERAL:
		context_str = "\033[1;37mGENERAL\033[0m | " + thread_id;
		break;
	case LogContext::TEST:
		context_str = "\033[1;37mTEST\033[0m | " + thread_id;
		break;
	case LogContext::SERVER_GROUP:
		context_str = "\033[1;33mSERVER_G\033[0m | " + thread_id;
		break;
	case LogContext::CLIENT_GROUP:
		context_str = "\033[1;35mCLIENT_G\033[0m | " + thread_id;
		break;
	case LogContext::GAME:
		context_str = "\033[1;35mGAME\033[0m | " + thread_id;
		break;

    default:
        context_str = thread_id;
        break;
    }

    // Add function name if available
    std::string function_info = "";
    if (!current_function.empty()) {
        function_info = "[" + current_function + "] ";
    }

    // Normal logs get standard indentation plus function name
    return context_str + function_info + message;
}

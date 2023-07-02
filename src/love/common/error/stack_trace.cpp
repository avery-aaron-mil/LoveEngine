#include "stack_trace.hpp"


namespace love_engine {
    std::string StackTrace::append_Stacktrace(const std::string& message) noexcept {
        std::stringstream buffer;
        buffer << message << "\n\nThrowing stack:\n" << get_Stacktrace();
        return buffer.str();
    }
    
    std::string StackTrace::append_Stacktrace(std::stringstream& buffer) noexcept {
        buffer << "\n\nThrowing stack:\n" << get_Stacktrace();
        return buffer.str();
    }
}
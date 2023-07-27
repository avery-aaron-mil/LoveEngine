#include <love/common/error/stack_trace.hpp>

namespace love_engine {
    std::string StackTrace::appendStacktrace(const std::string& message) noexcept {
        std::stringstream buffer;
        buffer << message << "\n\nThrowing stack:\n" << getStacktrace();
        return buffer.str();
    }
    
    std::string StackTrace::appendStacktrace(std::stringstream& buffer) noexcept {
        buffer << "\n\nThrowing stack:\n" << getStacktrace();
        return buffer.str();
    }
}
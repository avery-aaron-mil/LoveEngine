#include <love/common/error/stack_trace.hpp>

#include <sstream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>

#include <love/common/error/win32_error.hpp>
#else
#endif

namespace love_engine {
    std::string StackTrace::getStacktrace(const size_t skips, const size_t depth) noexcept {
        if (depth == 0) return std::string("Depth of zero passed to getStacktrace. You're here. Welcome. :)");
        std::stringstream buffer;

#ifdef _WIN32
        CONTEXT localContext {.ContextFlags = CONTEXT_FULL};
        CONTEXT* context = &localContext;
        RtlCaptureContext(context);
        if (context == nullptr) return std::string("Failed to get stack trace. Failed to get context record with RtlCaptureContext().");
        HANDLE thread = GetCurrentThread();
        if (thread == nullptr) return std::string("Failed to get stack trace. GetCurrentThread() failed.");
        HANDLE process = GetCurrentProcess();
        if (process == nullptr) return std::string("Failed to get stack trace. GetCurrentProcess() failed.");

        if (!SymInitialize(process, NULL, TRUE)) {
            buffer << "Failed to get stack trace. Failed to initialize symbol handler with SymInitialize(): "
                << Win32Error::getLastError();
            return buffer.str();
        }
        SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

        STACKFRAME64 stackFrame {};
        stackFrame.AddrStack.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrPC.Offset = context->Rip;
        stackFrame.AddrStack.Offset = context->Rsp;
        stackFrame.AddrFrame.Offset = context->Rbp;

        std::vector<void*> stacktrace;
        size_t i;
        for (i = 0; i < (skips + depth); ++i) {
            if (!StackWalk64(
                    IMAGE_FILE_MACHINE_AMD64,
                    process, thread, &stackFrame, context,
                    NULL,
                    SymFunctionTableAccess64, SymGetModuleBase64,
                    NULL
                )
            ) {
                buffer << "Failed to get stack trace. StackWalk64 failed.\n";
                return buffer.str();
            }

            if (stackFrame.AddrReturn.Offset == 0) break; // Reached end of stack

            if (i >= skips) stacktrace.push_back(reinterpret_cast<void *>(stackFrame.AddrPC.Offset));
        }
        buffer << "Size: " << stacktrace.size() << "\n";

        if (stacktrace.empty()) {
            buffer << "Failed to get stack trace. Reached a depth of " << i << " with " << skips << " skips.";
            return buffer.str();
        }

        SymCleanup(process);
#else
        // TODO
#endif

        return buffer.str();
    }
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
#pragma once
#include <core/Log.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <optional>
#include <source_location>
#include <string>

#ifdef __DEBUG_BUILD__
#define PIXL_LOG_AND_RETURN_ERROR(_returnValue_, _errorCode_, _module_, _fmt_, ...) \
    do                                                                              \
    {                                                                               \
        pixl::core::ErrorLog _pixl_err{                                             \
            (_errorCode_),                                                          \
            fmt::format(FMT_STRING(_fmt_), ##__VA_ARGS__),                          \
            (_module_),                                                             \
            std::source_location::current()};                                       \
        pixl::core::Errors::LogError(_pixl_err);                                    \
        return (_returnValue_);                                                     \
    } while (0)

#define PIXL_LOG_ERROR(_errorCode_, _module_, _fmt_, ...)  \
    do                                                     \
    {                                                      \
        pixl::core::ErrorLog _pixl_err{                    \
            (_errorCode_),                                 \
            fmt::format(FMT_STRING(_fmt_), ##__VA_ARGS__), \
            (_module_),                                    \
            std::source_location::current()};              \
        pixl::core::Errors::LogError(_pixl_err);           \
    } while (0)

#define PIXL_LOG_ERROR_WITH_CAUSE(_errorCode_, _module_, _cause_, _fmt_, ...) \
    do                                                                        \
    {                                                                         \
        pixl::core::ErrorLog _pixl_err{                                       \
            (_errorCode_),                                                    \
            fmt::format(FMT_STRING(_fmt_), ##__VA_ARGS__),                    \
            (_module_),                                                       \
            std::source_location::current(),                                  \
            (_cause_)};                                                       \
        pixl::core::Errors::LogError(_pixl_err);                              \
    } while (0)
#else
#define PIXL_LOG_AND_RETURN_ERROR(_returnValue_, _errorCode_, _module_, _fmt_, ...) \
    do                                                                              \
    {                                                                               \
        pixl::core::ErrorLog _pixl_err{                                             \
            (_errorCode_),                                                          \
            fmt::format(FMT_STRING(_fmt_), ##__VA_ARGS__),                          \
            (_module_),                                                             \
        };                                                                          \
        pixl::core::Errors::LogError(_pixl_err);                                    \
        return (_returnValue_);                                                     \
    } while (0)

#define PIXL_LOG_ERROR(_errorCode_, _module_, _fmt_, ...)  \
    do                                                     \
    {                                                      \
        pixl::core::ErrorLog _pixl_err{                    \
            (_errorCode_),                                 \
            fmt::format(FMT_STRING(_fmt_), ##__VA_ARGS__), \
            (_module_),                                    \
        };                                                 \
        pixl::core::Errors::LogError(_pixl_err);           \
    } while (0)

#define PIXL_LOG_ERROR_WITH_CAUSE(_errorCode_, _module_, _cause_, _fmt_, ...) \
    do                                                                        \
    {                                                                         \
        pixl::core::ErrorLog _pixl_err{                                       \
            (_errorCode_),                                                    \
            fmt::format(FMT_STRING(_fmt_), ##__VA_ARGS__),                    \
            (_module_),                                                       \
            std::source_location::current(),                                  \
            (_cause_)};                                                       \
        pixl::core::Errors::LogError(_pixl_err);                              \
    } while (0)
#endif
namespace pixl::core
{

    enum Errc : uint32_t
    {
        Ok = 0,
        UnknownError = 1,
        FileNotFound = 2,
        InvalidParameter = 3,
        UninitialisedSubsystem = 4,
        AlreadyInitialisedSubsystem = 5,
    };

    struct ErrorLog
    {
        Errc code{};
        std::string message;
        std::string_view module;
        std::source_location where = std::source_location::current();
        std::optional<std::string> cause;
    };

    class Errors
    {
    public:
        static int Init();
        static int Quit();
        static void LogError(const ErrorLog &error);

    private:
        static bool IsInitialized_;
    };
}
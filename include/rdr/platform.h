/**
 * @file platform.h
 * @author ShanghaiTech CS171 TAs
 * @brief Platform-specific (or not) definitions. Contains aliases for output
 * and some basic definitions. Students should respect the interface defined
 * here instead of using their own like `std::cout` which can result in
 * conflicts. Our logging library is `spdlog` and `fmt` for formatting, since
 * they are thread-safe and efficient.
 * @version 0.1
 * @date 2023-04-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <fmt/core.h>
#include <fmt/format.h>

#ifndef SPDLOG_FMT_EXTERNAL
#define SPDLOG_FMT_EXTERNAL
#endif
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

// ANSICOLOR_SINK is not supported on Windows
#if !defined(_WIN32)
#include <spdlog/sinks/ansicolor_sink.h>
#endif
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <optional>

#undef NDEBUG
#include <assert.h>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(__APPLE__)
#elif defined(__linux__)
#include <signal.h>
#include <sys/ptrace.h>
#endif

#define RDR_NAMESPACE_NAME renderer

#if !defined(RDR_NAMESPACE_BEGIN)
#define RDR_NAMESPACE_BEGIN namespace renderer {
#endif

#if !defined(RDR_NAMESPACE_END)
#define RDR_NAMESPACE_END }  // namespace renderer
#endif

#define RDR_FORCEINLINE inline

// TODO
#define UNIMPLEMENTED assert(false)

template <typename... T>
RDR_FORCEINLINE decltype(auto) print(T &&...args) {
  return fmt::print(std::forward<T>(args)...);
}

template <typename... T>
RDR_FORCEINLINE decltype(auto) format(T &&...args) {
  return fmt::format(std::forward<T>(args)...);
}

// Add suffix to avoid conflicts with other libraries.
struct rdr_exception : public std::runtime_error {  // NOLINT
  using std::runtime_error::runtime_error;
};

#define Info_(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#define Warn_(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#define Error_(...) \
  SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#define Exception_(...)                       \
  do {                                        \
    throw rdr_exception(format(__VA_ARGS__)); \
  } while (false)
namespace fs = std::filesystem;

RDR_NAMESPACE_BEGIN

// TODO: move to std.h
template <typename T>
using vector = std::vector<T>;

template <typename T>
using optional = std::optional<T>;

RDR_FORCEINLINE void InitLogger(bool b_use_err = false, bool b_quite = false) {
  spdlog::default_logger_raw()->sinks().clear();
#if !defined(_WIN32)
  if (b_use_err)
    spdlog::default_logger_raw()->sinks().push_back(
        std::make_shared<spdlog::sinks::ansicolor_stderr_sink_mt>());
  else
    spdlog::default_logger_raw()->sinks().push_back(
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif
  spdlog::set_pattern("[%^%l%$] %v");
  if (b_quite) spdlog::set_level(spdlog::level::err);
}
// 这个initlogger函数是用来初始化日志系统的。它配置了日志的输出方式和日志级别，确保在程序运行过程中能够正确地记录和显示日志信息。具体来说，这个函数做了以下几件事：
// 1.清除默认日志记录器的所有输出接收器（sinks），以便重新配置日志输出方式。
// 2.根据传入的参数b_use_err，决定日志是输出到标准错误（stderr）还是标准输出（stdout）。如果b_use_err为true，日志将输出到stderr，否则输出到stdout。
// 3.设置日志的输出格式模式，这里使用了一个简单的模式，显示日志级别和消息内容。
// 4.如果传入的参数b_quite为true，设置日志级别为错误级别（err），这样只有错误日志会被输出，其他级别的日志将被忽略。
// 通过调用这个函数，程序可以根据需要灵活地配置日志系统，方便调试和监控程序的运行状态。

RDR_FORCEINLINE void RaiseDebugger() {
#if defined(_WIN32)
  if (IsDebuggerPresent()) __debugbreak();
#elif defined(__APPLE__)
  // TODO
#elif defined(__linux__)
  // TODO
  // if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) raise(SIGTRAP);
#endif
}

RDR_NAMESPACE_END

#endif

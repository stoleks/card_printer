#include "log.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// initialize static member
Logger::PtrLogger Logger::mLogger;

/////////////////////////////////////////////////
void Logger::Init (
  const std::string& filename,
  const Logger::Output output,
  const Logger::Level level)
{
  spdlog::set_pattern ("%^[%T]: %v%$");

  // set output of the log
  if (output == Output::File) {
    mLogger = spdlog::basic_logger_mt ("App", filename);
  }
  if (output == Output::Console) {
    mLogger = spdlog::stdout_color_mt ("App");
  }

  // set log level
  switch (level) {
    case Level::Error: mLogger->set_level (spdlog::level::err);   break;
    case Level::Warn:  mLogger->set_level (spdlog::level::warn);  break;
    case Level::Info:  mLogger->set_level (spdlog::level::info);  break;
    default:           mLogger->set_level (spdlog::level::trace); break;
  }
}

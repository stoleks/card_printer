/**
  Log.h
  Purpose: implement a flexible log system which allow
    messages to be logged into console, file, window
    or combination of the three. Code inspired/taken
    from xygine : https://github.com/fallahn/xygine
  @author A. J.
*/

#pragma once

#include <memory>
#include "spdlog/spdlog.h"

class Logger
{
public:
  /**
   * type of output
   */
  enum class Output {
    Console,
    File
  };
  /**
   * level of log: Error we only print error, Warn we print
   * both warning and error, etc. Debug print everything
   */
  enum class Level {
    Error,
    Warn,
    Info,
    Debug
  };
  // to ease reading
  using PtrLogger = std::shared_ptr <spdlog::logger>;
public:
  /**
   * initialize our global logger
   */
  static void Init (
    const std::string& filename,
    const Output output = Output::Console,
    const Level level = Level::Debug);
  /**
   * get access to the logger
   */
  inline static PtrLogger& GetLogger () { return mLogger; }
private:
  static PtrLogger mLogger;
};

/**
 * print common message
 */
#define LogInfo(...)  Logger::GetLogger ()->info (__VA_ARGS__)
#define LogWarn(...)  Logger::GetLogger ()->warn (__VA_ARGS__)
#define LogError(...) Logger::GetLogger ()->error (__VA_ARGS__)
#define LogDebug(...) Logger::GetLogger ()->debug (__VA_ARGS__)

#ifdef _MSC_VER
  #define FuncName __FUNCSIG__
#else
  #define FuncName __PRETTY_FUNCTION__
#endif

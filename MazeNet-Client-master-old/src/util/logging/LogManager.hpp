#ifndef LOGMANAGER_HPP
#define LOGMANAGER_HPP

// stl
#include <string>
#include <unordered_set>

// boost
#include <boost/utility.hpp>
#include <boost/chrono.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>

namespace src = boost::log::sources;

namespace mazenet {
namespace util {
namespace logging {

/**
* @brief log message severity levels for filtering
*/
enum SeverityLevel {
  trace,         ///< used by function_tracer
  debug,
  info,          ///< general info messages
  notification,  ///< notifications about normally working events
  warning,       ///< warnings about unexpected event
  error,         ///< errors that resulted in a partial crash
  critical,      ///< error that caused a full crash
  none           ///< no logging
};

/**
* @brief global LogManager implemented based on Boost/Log
*/
class LogManager : boost::noncopyable {
 private:
  friend class Log;

  friend class CommandProfileLog;

  /**
  * returns the logManager Singleton instance
  * @return logManager
  */
  static LogManager& instance();

  /**
  * Writes a log message.
  * @param logName Name of target log
  * @param message The message to write
  * @param file The file from where the logger is called
  * @param line The line from where the logger is called.
  * @param func The function from where the logger is called.
  * @param severity Severity level of log message
  */
  void writeMessage(std::string const& logName,
                    std::string const& message,
                    std::string const& file,
                    unsigned int line,
                    const char* func,
                    SeverityLevel severity);

  /**
  * registers a new Log
  * @param name filename of the new file
  */
  bool addLog(std::string const& name);

  /**
  * registers a new Log using a custom formatter
  * @param name filename of the new file
  * @param fmt file formatting rules
  */
  bool addLog(std::string const& name, boost::log::formatter fmt);

  /**
  * default constructor
  */
  LogManager();

  /**
  * default destructor
  */
  virtual ~LogManager() = default;

  /**
  * initialise
  */
  void init();

  /**
  * logging backend
  */
  src::severity_logger_mt<SeverityLevel> logger_;

  /** The mutex used to make the logging threadsafe */
  mutable boost::mutex logMutex_;

  /** default global formatter */
  boost::log::formatter fmt_;

  /** The set containing all used logs */
  std::unordered_set<std::string> registeredLogs_;

  /** The time object to get the current time */
  boost::posix_time::ptime now_;

};
}  // namespace logging
}  // namespace util
}  // namespace mazenet

#endif /* LOGMANAGER_HPP */

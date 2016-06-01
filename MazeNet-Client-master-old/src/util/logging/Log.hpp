#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <boost/utility.hpp>
#include <sstream>


#include "LogManager.hpp"
// Some defines to make logging easy. These will automatially log the file line and function from where a logger is called.
#define logMessageSeverity(message, severity) logMessageFull(message,__FILE__,__LINE__,__FUNCTION__,severity)
#define logMessage(message) logMessageFull(message,__FILE__,__LINE__,__FUNCTION__)
#define log() logStream(__FILE__,__LINE__, __FUNCTION__)
#define logSeverity(severity) logStream(__FILE__,__LINE__,__FUNCTION__, severity)

namespace mazenet {
namespace util {
namespace logging {

/**
* This empty struct is used by the stream operator << to signal the end of an input.
*/
struct LogEnd {
};

/**
* Logger for easy filebased logging that uses LogManager as a backend
*/
class Log : boost::noncopyable {
 public:

  /**
  * constructor
  * @param name logging category name
  */
  Log(std::string const& name);

  /**
  * defaulted destructor
  */
  virtual ~Log() = default;

  /**
  * This method is used to create the beginning of a log stream.
  *
  * @param file The File where this method was called.
  * @param line The Line where this method was called.
  * @param func The Function where this method was called.
  * @param severity Severity of log message
  * @return Log Returns a reference to this Log object.
  */
  Log& logStream(const char* file,
                 unsigned int line,
                 const char* func,
                 SeverityLevel severity = SeverityLevel::notification);

  /**
  * The end Operator used to signal the end of an input.
  * returns LogEnd End of Input Signal
  */
  LogEnd end();


  /**
  * Write a message to the logfile
  * @param message The message to log.
  * @param file The File where this method was called.
  * @param line The Line where this method was called.
  * @param func The Function where this method was called.
  * @param severity Severity of log message
  */
  void logMessageFull(std::string const& message,
                      const char* file,
                      unsigned int line,
                      const char* func,
                      SeverityLevel severity = SeverityLevel::notification) const;

  /**
  * The Operator << is used to show the end of an input.
  * It triggers the write mechanism.
  * The returntype void prohibits the use of another << operator after an end signal was received.
  */
  void operator<<(const LogEnd&);

 protected:
  /** Default constuctor. */
  Log() = default;

  /** Name of this specific logger */
  std::string name_ = "";

  /** Boolean used to control the activity of this logger */
  bool enabled_ = false;


  // Information needed for the << Operator.
  /** The file from where the log operation is called */
  std::string file_ = "";
  /** The line from where the log operation is called */
  unsigned int line_ = 0;
  /** The function from where the log operation is called */
  std::string func_ = "";
  /** The serverity level used in this log process */
  SeverityLevel severity_ = SeverityLevel::notification;
  /** The buffer used to store the intel */
  std::stringstream buffer_{};

  // Declaring the method as friend so that we can use the string buffer
  template<typename T>
  friend Log& operator<<(Log& l, const T& param);
};

/**
* Writes the parameter into the logstream.
* The parameter itself need to be writable into an stringstream.
*/
template<typename T>
Log& operator<<(Log& l, const T& param) {
  l.buffer_ << param;
  return l;
}

} // namespace logging
} // namespace util
} // namespace mazenet

#endif /* LOG_HPP */

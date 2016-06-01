#include "Log.hpp"
#include <boost/filesystem.hpp>

namespace mazenet {
namespace util {
namespace logging {

/***************************************************************/

Log::Log(std::string const& name) : name_(name), enabled_(LogManager::instance().addLog(name)) {

}

LogEnd Log::end() {
  return LogEnd{};
}

Log& Log::logStream(const char* file,
                    unsigned int line,
                    const char* func,
                    SeverityLevel severity) {
  // Remembering the information for further use.
  file_ = file;
  line_ = line;
  func_ = func;
  severity_ = severity;
  buffer_.str("");
  return *this;
}

/***************************************************************/

void Log::logMessageFull(std::string const& message,
                         const char* file,
                         unsigned int line,
                         const char* func,
                         SeverityLevel severity) const {
  if (enabled_) {
    LogManager::instance().writeMessage(name_,
                                        message,
                                        boost::filesystem::path(file).filename().string(),
                                        line,
                                        func,
                                        severity);
  }
}

void Log::operator<<(const LogEnd&) {
  // When receiving the end signal, information will be written into the log.
  logMessageFull(buffer_.str(),
                 file_.c_str(),
                 line_,
                 func_.c_str(),
                 severity_);
  // Maybe resetting the variables will free some memory
}

} // namespace logging
} // namespace util
} // namespace mazenet

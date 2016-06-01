// header file
#include "LogManager.hpp"

#include <Constants.hpp>
#include <util/cfg/CfgManager.hpp>

// stl

// boost
#include <boost/thread.hpp>
#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <boost/filesystem.hpp>

#include <boost/date_time.hpp>

// define a few useful namespaces
namespace attrs    = boost::log::attributes;
namespace expr     = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace sinks    = boost::log::sinks;
namespace src      = boost::log::sources;

namespace mazenet {
namespace util {
namespace logging {

// BOOST log defines
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SeverityLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_name, "Name", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(line, "Line", unsigned)
BOOST_LOG_ATTRIBUTE_KEYWORD(func, "Function", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(count, "Count", unsigned)
BOOST_LOG_ATTRIBUTE_KEYWORD(time_elapsed, "Elapsed", double)
BOOST_LOG_ATTRIBUTE_KEYWORD(time_average, "Average", double)

/****************************************************************/

std::ostream& operator<<(std::ostream& strm, SeverityLevel level) {
  constexpr static const char* strings[] = {
      "trace",
      "debug",
      "info",
      "notification",
      "warning",
      "error",
      "critical",
      "none"
  };

  if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
    strm << strings[level];
  else
    strm << static_cast<int>(level);

  return strm;
}

/****************************************************************/

LogManager& LogManager::instance() {
  static LogManager logMan;
  return logMan;
}

/***************************************************************/

void LogManager::writeMessage(std::string const& name,
                              std::string const& message,
                              std::string const& file,
                              unsigned int line,
                              const char* func,
                              SeverityLevel severity) {
  src::logger_mt lg;
  lg.add_attribute("Name", attrs::constant<std::string>(name));
  lg.add_attribute("Severity", attrs::constant<SeverityLevel>(severity));
  lg.add_attribute("File", attrs::constant<std::string>(file));
  lg.add_attribute("Line", attrs::constant<unsigned>(line));
  lg.add_attribute("Function", attrs::constant<std::string>(func));
  lg.add_attribute("TimeStamp",
                   attrs::constant<boost::posix_time::ptime>(boost::posix_time::second_clock::local_time()));
  BOOST_LOG(lg) << message;
}

/****************************************************************/

LogManager::LogManager() :
    logger_(), logMutex_(), fmt_(), registeredLogs_(), now_(boost::posix_time::second_clock::local_time()) {
  init();
}

/****************************************************************/

void LogManager::init() {
  std::stringstream logdir;

  boost::posix_time::time_facet* f = new boost::posix_time::time_facet("%Y_%m_%d_%H_%M_%S%F%q");
  logdir.imbue(std::locale(std::cout.getloc(), f));
  logdir << cLogDir << "/" << now_;
  mazenet::util::cfg::CfgManager::instance().set("log.directory",
                                                 logdir.str(),
                                                 false);

  if (!boost::filesystem::exists(cLogDir)) {
    boost::filesystem::create_directory(cLogDir);
  }

  if (!boost::filesystem::exists(logdir.str())) {
    boost::filesystem::create_directory(logdir.str());
  }

  // Setup standard file formatting
  fmt_ =
      expr::stream << std::setw(10) << std::setfill('.') << line_id << std::setfill(' ') << ": [" << severity
      << "] " << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
      << " (" << file << ":" << line << " --> " << func << ")" << std::endl << "\t"
      << expr::if_(expr::has_attr(
          tag_attr))[expr::stream << "<" << tag_attr << "> "]
      << expr::smessage;

  if (mazenet::util::cfg::CfgManager::instance().get<bool>("log.verbose", false)) {
    // initialize sinks
    typedef sinks::synchronous_sink <sinks::text_ostream_backend> text_sink;

    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

    // add error log streams
    sink = boost::make_shared<text_sink>();

    // add std::cout stream for Debug logging
    sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(
        &std::cout, boost::serialization::null_deleter()));

    sink->locked_backend()->auto_flush(true);


    sink->set_filter((expr::has_attr(severity) &&
                      (severity >= mazenet::util::cfg::CfgManager::instance().get<int>("log.verbose_level", 0))));
    boost::log::core::get()->add_sink(sink);
  }
  boost::log::add_common_attributes();
}

/****************************************************************/

bool LogManager::addLog(std::string const& name) {
  return addLog(name, fmt_);
}

/****************************************************************/

bool LogManager::addLog(std::string const& name, boost::log::formatter fmt) {
  if (mazenet::util::cfg::CfgManager::instance().get<bool>("log." + name, true)) {
    typedef sinks::synchronous_sink <sinks::text_ostream_backend> text_sink;

    unsigned logLevel = mazenet::util::cfg::CfgManager::instance().get<unsigned>("log.level", SeverityLevel::info);

    if (logLevel < SeverityLevel::none) {

      boost::unique_lock<boost::mutex> lock(logMutex_);
      if (!registeredLogs_.count(name)) {
        registeredLogs_.insert(name);
        lock.unlock();

        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        std::stringstream ss;
        boost::posix_time::time_facet* f = new boost::posix_time::time_facet("%Y_%m_%d_%H_%M_%S%F%q");
        ss.imbue(std::locale(std::cout.getloc(), f));
        ss << mazenet::util::cfg::CfgManager::instance().get<std::string>("log.directory") << "/" << name << ".log";

        // add log stream
        sink->locked_backend()->add_stream(
            boost::make_shared<std::ofstream>(ss.str()));
        sink->set_formatter(fmt);
        sink->locked_backend()->auto_flush(true);

        sink->set_filter((expr::has_attr(severity) && (severity >= logLevel)) &&
                         (expr::has_attr(log_name) && log_name == name));

        boost::log::core::get()->add_sink(sink);
      }
      return true;
    }
  }
  return false;
}

/***************************************************************/

}  // namespace logging
}  // namespace util
}  // namespace mazenet

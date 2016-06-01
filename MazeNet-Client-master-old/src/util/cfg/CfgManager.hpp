/*
* contains the CfgManager class
* this manager is currently focused on functionality and has optimization potential
*/
#ifndef CFG_MANAGER_HPP
#define CFG_MANAGER_HPP

// boost
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/noncopyable.hpp>

// mazenet
#include "ProgramOptions.hpp"
#include "JSONConfig.hpp"
#include "CfgException.hpp"

namespace mazenet {
namespace util {
namespace cfg {

/**
* global configuration manager.
*/
class CfgManager : boost::noncopyable {
 public:
  /**
  * singleton instance.
  * @return CfgManager instance
  */
  static CfgManager& instance();

  /**
  * parses the commandline options and places them in a property tree.
  * @param argc commandline argument count
  * @param argv commandline argument array
  * @return bool continue program execution
  */
  ExecutionMode parseCmdLineOptions(int argc, char* argv[]);

  /**
  * retrieves a value from the property tree.
  * @tparam T variable type
  * @param propertyName name of the requested property
  * @return property value
  */
  template<typename T>
  T get(std::string const& propertyName) const throw(PropertyNotFoundException) {
    using namespace boost;

    T returnValue;
    lock_guard<recursive_mutex> lock(mutex_);
    if (overrides_.try_get(propertyName, returnValue)) {
      return returnValue;
    }

    if (configuration_.try_get(propertyName, returnValue)) {
      return returnValue;
    }

    BOOST_THROW_EXCEPTION(PropertyNotFoundException(propertyName));
  }

  /**
  * retrieves a value from the property tree and defines a defaultValue
  * @tparam T variable type
  * @param propertyName name of the requested property
  * @param defaultValue default value if property is not found
  * @return property value
  */
  template<typename T>
  T get(std::string const& propertyName, T defaultValue) const noexcept {
    using namespace boost;
    T returnValue;
    lock_guard<recursive_mutex> lock(mutex_);
    if (overrides_.try_get(propertyName, returnValue)) {
      return returnValue;
    }

    if (configuration_.try_get(propertyName, returnValue)) {
      return returnValue;
    }
    return defaultValue;
  }

  /**
  * Sets a value in the property tree
  * @tparam T variable type
  * @param propertyName name of the property
  * @param value new property value
  * @param persistent store as a persistent configuration value
  * @return property value
  */
  template<typename T>
  void set(std::string const& propertyName, T value, bool persistent) {
    using namespace boost;
    lock_guard<recursive_mutex> lock(mutex_);
    if (persistent) {
      configuration_.set(propertyName, value);
    } else {
      overrides_.set(propertyName, value);
    }
  }

  template<typename T>
  void set_collection(std::string const& propertyName, T values, bool persistent) {
    using namespace  boost;
    lock_guard<recursive_mutex> lock(mutex_);
    if (persistent) {
      configuration_.set_collection(propertyName, values);
    } else {
      overrides_.set_collection(propertyName, values);
    }
  }

  template<typename colType>
  colType get_collection(std::string const& propertyName) {
    using namespace boost;
    colType returnValue;
    lock_guard<recursive_mutex> lock(mutex_);
    if (overrides_.try_get_collection(propertyName, returnValue)) {
      return returnValue;
    }

    if (configuration_.try_get_collection(propertyName, returnValue)) {
      return returnValue;
    }
    return colType();
  }

  /**
  * saves the configuration to config.json
  */
  void save() const;

  /**
  * Creates the default configuration.
  */
  void createDefault();

  /**
  * dumps the overrides into a file
  * @param filename name of the configuration file
  */
  void dumpOverrides(std::string const& filename) const;

 private:
  /**
  * converts the startup parameters into overrides.
  */
  void translateCmdOpts();

  void getExecutionMode(ExecutionMode& mode);

  /** private constructor, this is a singleton */
  CfgManager();

  /**
  * private destructor, this is a singleton
  * saves the configuration
  */
  virtual ~CfgManager();

  /** mutex for threadsafe access */
  mutable boost::recursive_mutex mutex_;

  /** non persistent commandline options parser */
  ProgramOptions cmdOpts_;
  /** non persistent runtime overrides */
  JSONConfig overrides_;
  /** persistent configuration options */
  JSONConfig configuration_;
};
} // namespace cfg
} // namespace util
} // namespace mazenet

#endif // CFG_MANAGER_HPP

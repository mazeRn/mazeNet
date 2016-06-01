#ifndef JSON_CONFIG_HPP
#define JSON_CONFIG_HPP
#pragma once

// stl
#include <string>

// boost
#include <boost/utility.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "CfgException.hpp"

namespace mazenet {
namespace util {
namespace cfg {
/**
* @brief JSONConfig filehandler
* This JSONConfig class can load and store json files and manage
* the options in a property tree
*/
class JSONConfig {
 public:
  /**
  * constructor
  */
  JSONConfig();

  /**
  * constructor
  */
  JSONConfig(std::string const& filename);

  JSONConfig(const JSONConfig&) = delete;

  JSONConfig(JSONConfig&&) = default;

  JSONConfig& operator=(const JSONConfig&) = delete;

  JSONConfig& operator=(JSONConfig&&) = default;

  /**
  * default destructor
  */
  virtual ~JSONConfig() = default;

  /**
  * load a configuration tree from a xml configuration file
  * this function should only be called once on initialization
  * @param filename name of xml file
  */
  void load(const std::string& filename) throw(ParserErrorException);

  /**
  * stores the configuration tree to an xml configuration file
  * this function should be called on shutdown and for permanent configuration
  * storage
  */
  void store(const std::string& filename) const throw(ParserErrorException);

  /**
  * get a value from the tree
  * @tparam T value type
  * @param propertyName name of the property (path.to.property)
  * @return property value
  */
  template<class T>
  T get(const std::string& propertyName) const {
    return pt_.get<T>(propertyName);
  }

  /**
  * get a value from the tree
  * @tparam T value type
  * @param propertyName name of the property (path.to.property)
  * @param defaultValue default value if property is not present
  * @return property value
  */
  template<class T>
  T get(const std::string& propertyName, T defaultValue) const {
    return pt_.get(propertyName, defaultValue);
  }

  /**
  * store/update a value in the tree
  * @tparam T value type
  * @param propertyName name of the property(path.to.property)
  * @param value new value of the property
  */
  template<class T>
  void set(const std::string& propertyName, T value) {
    pt_.put(propertyName, value);
  }

  /**
  * get a value from the tree without throwing an exception
  * @tparam T value type
  * @param propertyName name of the property (path.to.property)
  * @param returnValue retrieved value
  * @return value successfully retrieved
  */
  template<class T>
  bool try_get(const std::string& propertyName, T& returnValue) const noexcept {
    try {
      returnValue = pt_.get<T>(propertyName);
      return true;
    }
    catch (...) {
      return false;
    }
  }

  /**
  * get a value from the tree without throwing an exception
  * @tparam T value type
  * @param propertyName name of the property (path.to.property)
  * @param defaultValue default value if value is not present
  * @param returnValue retrieved value
  * @return value successfully retrieved
  */
  template<class T>
  bool try_get(const std::string& propertyName, T defaultValue, T& returnValue) const {
    returnValue = pt_.get<T>(propertyName, defaultValue);
    return (bool) pt_.count(propertyName);
  }

  template<typename T>
  void set_collection(std::string const& propertyName, T values) {
    boost::property_tree::ptree node;
    boost::property_tree::ptree array;
    for (auto& val: values) {
      node.put_value(val);
      array.push_back(std::make_pair("", node));
    }
    pt_.put_child(propertyName, array);
  }

  template<typename colType>
  bool try_get_collection(std::string const& propertyName, colType& returnValue) {
    try {
      for (auto& item : pt_.get_child(propertyName))
        returnValue.push_back(item.second.get_value<typename colType::value_type>());
      return true;
    } catch (const boost::exception& ex) {
      // silent catch
    }
    return false;
  }

 protected:
 private:
  /**tree containing values */
  boost::property_tree::ptree pt_;
};

}  // namespace cfg
}  // namespace util
}  // namespace mazenet

#endif  // JSON_CONFIG_HPP

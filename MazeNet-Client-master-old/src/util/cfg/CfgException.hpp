#ifndef CFG_EXCEPTION_HPP
#define CFG_EXCEPTION_HPP

#include <exception/Exception.hpp>

namespace mazenet {
namespace util {
namespace cfg {

class CfgException : virtual public mazenet::Exception {
 public:
  virtual const char* what() const noexcept override {
    return "Error in CfgManager";
  }
};

class PropertyNotFoundException : virtual public mazenet::Exception {
 public:
  PropertyNotFoundException(std::string property_name) : property_name_(property_name) {

  }

  virtual const char* what() const noexcept override {
    return property_name_.c_str();
  }
 private:
  std::string property_name_;
};

class ParserErrorException : virtual public mazenet::Exception {
 public:
  virtual const char* what() const noexcept override {
    return "Error while parsing configuration file";
  }
};

}
}
}

#endif // CFG_EXCEPTION_HPP

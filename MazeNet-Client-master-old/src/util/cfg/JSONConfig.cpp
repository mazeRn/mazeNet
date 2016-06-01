#include "JSONConfig.hpp"

// boost
#include <boost/filesystem.hpp>

namespace mazenet {
namespace util {
namespace cfg {

/****************************************************************/

JSONConfig::JSONConfig() : pt_() {

}

/****************************************************************/

JSONConfig::JSONConfig(std::string const& filename) : pt_() {
  load(filename);
}

/****************************************************************/

void JSONConfig::load(const std::string& filename) throw(ParserErrorException) {
  try {
    boost::property_tree::json_parser::read_json(filename, pt_);
  } catch (boost::property_tree::json_parser::json_parser_error) {
    throw ParserErrorException();
  }
}  // JSONConfig::load

/****************************************************************/

void JSONConfig::store(const std::string& filename) const throw(ParserErrorException) {
  try {
    if (boost::filesystem::exists(filename))
      boost::filesystem::remove(filename);

    boost::property_tree::json_parser::write_json(filename, pt_, std::locale());
  } catch (boost::property_tree::json_parser::json_parser_error) {
    throw ParserErrorException();
  }
}  // JSONConfig::store

/****************************************************************/

}  // namespace cfg
}  // namespace util
}  // namespace mazenet

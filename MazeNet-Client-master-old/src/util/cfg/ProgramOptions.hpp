#ifndef PROGRAM_OPTIONS_HPP
#define PROGRAM_OPTIONS_HPP

#include <boost/program_options.hpp>
//#include <boost/functional.hpp>

#include <iostream>

#include "Config.hpp"

namespace po = boost::program_options;

namespace mazenet {
namespace util {
namespace cfg {

enum class ExecutionMode {
  NONE,
  EXIT,
  RUN
};

/**
* wrapper around boost program_options.
* only used inside of CfgManager.
*/
class ProgramOptions {
 private:
  // this should never be used outside of CfgManager
  friend class CfgManager;

  ProgramOptions();

  /**
  * initial parse of commandline options
  */
  void init_options(int argc, char* argv[]);

  /**
  * print the commandline help
  */
  inline void print_help(void) {
    std::cout << cmdline_options_ << std::endl;
  }

  /**
  * prints the version info
  */
  inline void print_version(void) {
    std::cout << "DarkDev MazeNet Client" << std::endl;
    std::cout << "Version: " << PROJECT_MAJOR_VERSION << "." << PROJECT_MINOR_VERSION << std::endl;
    std::cout << "Revision: " << REVISION_NUMBER << std::endl;
    std::cout << "Compiled using: " << COMPILER_VERSION << std::endl;
  }

  /**
  * execute parameter related methods
  * @return continue execution
  */
  ExecutionMode execute();

  /** commandline options */
  po::options_description cmdline_options_;

  /** internal variable map */
  po::variables_map vm_;
};

}
}
}

#endif // PROGRAM_OPTIONS_HPP

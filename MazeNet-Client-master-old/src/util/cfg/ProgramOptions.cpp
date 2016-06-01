#include "ProgramOptions.hpp"

namespace mazenet {
namespace util {
namespace cfg {

ProgramOptions::ProgramOptions() : cmdline_options_(), vm_() {

}

void ProgramOptions::init_options(int argc, char* argv[]) {
  /** description of general category */
  po::options_description general("General");
  general.add_options()
             ("host,h", po::value<std::string>(), "specify hostname")
             ("port,p", po::value<std::string>(), "specify port")
             ("name,n", po::value<std::string>(), "specify username")
             ("timeout,t", po::value<int>(), "server timeout in seconds")
             ("version,v", "display program version")
             ("help", "display help message");

  /** description of debug category */
  po::options_description debug("Debug");
  debug.add_options()
           ("verbose", "display verbose console output")
           ("loglevel", po::value<unsigned>(), "minimum level required for log triggering \n 0[info] - 5[critical]")
           ("default-settings", "restore default configuration")
           ("dump-overrides", "dumps all overrides");


  cmdline_options_.add(general).add(debug);

  try {
    po::store(po::command_line_parser(argc, argv).options(cmdline_options_).run(), vm_);
    po::notify(vm_);
  } catch (...) {

  }
}

ExecutionMode ProgramOptions::execute() {
  if (vm_.count("help")) {
    print_help();
    return ExecutionMode::EXIT;
  }
  if (vm_.count("version")) {
    print_version();
    return ExecutionMode::EXIT;
  }

  return ExecutionMode::RUN;
}

}
}
}

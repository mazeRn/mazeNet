#include <Constants.hpp>
#include "CfgManager.hpp"

namespace mazenet {
namespace util {
namespace cfg {

/****************************************************************/

CfgManager::CfgManager() : mutex_(), cmdOpts_(), overrides_(), configuration_() {
  try {
    configuration_.load(cCfgFile);
  } catch (...) {
    std::cout << "critical initialization error, config invalid please create "
        "a default one, or check the existing configuration for errors"
    << std::endl;
    throw mazenet::Exception();
  }
}

/****************************************************************/

CfgManager& CfgManager::instance() {
  static CfgManager instance_;
  return instance_;
}

/****************************************************************/

CfgManager::~CfgManager() {
}

/****************************************************************/

ExecutionMode CfgManager::parseCmdLineOptions(int argc, char* argv[]) {
  using namespace boost;
  lock_guard<recursive_mutex> lock(mutex_);
  cmdOpts_.init_options(argc, argv);
  ExecutionMode mode = cmdOpts_.execute();
  translateCmdOpts();
  getExecutionMode(mode);
  return mode;
}

/****************************************************************/

void CfgManager::translateCmdOpts() {
  // translation of commandline options into overrides_

  // Order like in config.json
  if (cmdOpts_.vm_.count("host")) {
    overrides_.set("server.host", cmdOpts_.vm_["host"].as<std::string>());
  }

  if (cmdOpts_.vm_.count("port")) {
    overrides_.set("server.port", cmdOpts_.vm_["port"].as<std::string>());
  }

  if (cmdOpts_.vm_.count("name")) {
    overrides_.set("player.name", cmdOpts_.vm_["name"].as<std::string>());
  }

  if (cmdOpts_.vm_.count("verbose")) {
    overrides_.set("log.verbose", true);
  }

  if (cmdOpts_.vm_.count("loglevel")) {
    overrides_.set("log.level", cmdOpts_.vm_["loglevel"].as<unsigned>());
  }
  // End of config.json overrides

  if (cmdOpts_.vm_.count("default-settings")) {
    createDefault();
  }

  if (cmdOpts_.vm_.count("dump-overrides")) {
    dumpOverrides(cCfgOverrideFile);
  }
}

/****************************************************************/

void CfgManager::getExecutionMode(ExecutionMode& mode) {

}

/****************************************************************/

void CfgManager::createDefault() {
  using namespace boost;
  lock_guard<recursive_mutex> lock(mutex_);

  // server category
  configuration_.set<std::string>("server.host", "127.0.0.1");
  configuration_.set<std::string>("server.port", "5123");
  configuration_.set<int>("server.timeout", 60);
  configuration_.set<unsigned>("server.retry", 5);

  // player category
  configuration_.set<std::string>("player.name", "DarkDev");

  // log category
  configuration_.set<bool>("log.main", true);
  configuration_.set<bool>("log.network", false);
  configuration_.set<bool>("log.verbose", false);
  configuration_.set<int>("log.level", 0);

  save();
}

/****************************************************************/

void CfgManager::dumpOverrides(std::string const& filename) const {
  using namespace boost;
  lock_guard<recursive_mutex> lock(mutex_);
  overrides_.store(filename);
}

/****************************************************************/

void CfgManager::save() const {
  using namespace boost;
  lock_guard<recursive_mutex> lock(mutex_);
  configuration_.store(cCfgFile);
}

/****************************************************************/

}
}
}

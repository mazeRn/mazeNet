/**
 * @file main.cpp
 * @author Dark Dev
 * @date 27-05-2015
 * @brief
 */

#include <sstream>
#include <messaging/MessageDispatcher.hpp>
#include <player/HeuristicStrategy.hpp>

#include "network/TcpClient.hpp"
#include "util/cfg/CfgManager.hpp"
#include "util/logging/Log.hpp"
#include "messaging/mazeCom.hxx"
#include "messaging/MessageHandler.hpp"

int main(int argc, char *argv[]) {
  xercesc::XMLPlatformUtils::Initialize();


  using namespace mazenet::util::cfg;
  using namespace mazenet::util::logging;
  CfgManager& cfgMan = CfgManager::instance();

  // keep as first line
  if (cfgMan.parseCmdLineOptions(argc, argv) == ExecutionMode::RUN) {
    Log logger("main");

    TcpClient client;
    auto con = client.getConnection();
    auto host = cfgMan.get<std::string>("server.host");
    auto port = cfgMan.get<std::string>("server.port");

    logger.log() << "Connecting to: " << host << ":" << port << logger.end();
    client.openConnection(host, port);

    MessageDispatcher dispatcher(client.getConnection());
    MessageHandler handler(std::make_shared<HeuristicStrategy>(), dispatcher);

    client.getConnection()->setReadHandler([&handler](const std::string& msg) {
        handler.handle_incoming_message(msg);
      });

    dispatcher.sendLoginMessage(cfgMan.get<std::string>("player.name"));

    client.getIOService().run();
  }

  xercesc::XMLPlatformUtils::Terminate();
  return 0;
}

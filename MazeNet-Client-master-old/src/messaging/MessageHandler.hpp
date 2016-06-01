#ifndef MAZENET_CLIENT_MESSAGEHANDLER_HPP
#define MAZENET_CLIENT_MESSAGEHANDLER_HPP

#include <string>
#include <player/IPlayerStrategy.hpp>

#include "../messaging/mazeCom.hxx"
#include "../player/GameSituation.hpp"
#include "../util/logging/Log.hpp"
#include "MessageDispatcher.hpp"



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" //Ignoring the warning.
class MessageHandler {
 public:
  MessageHandler(std::shared_ptr<IPlayerStrategy> strategy, MessageDispatcher& dispatcher);

  void handle_incoming_message(const std::string& message);

 private:
  void handle_login_reply(const LoginReplyMessageType& reply);

  void handle_await_move(const AwaitMoveMessageType& await_move);

  void handle_accept_message(const AcceptMessageType& accept_message);

  void handle_win_message(const WinMessageType& win_message);

  void handle_disconnect_message(const DisconnectMessageType& disconnect_message);

  void update_model(const AwaitMoveMessageType& message);

  void update_board(const boardType& board);

  std::shared_ptr<MazeCom> deserialize(const std::string& msg);

  std::shared_ptr<IPlayerStrategy> strategy_;
  mazenet::util::logging::Log logger_;

  MessageDispatcher& dispatcher_;

  xsd::cxx::tree::error_handler<char> eh;
  xsd::cxx::xml::dom::bits::error_handler_proxy<char> ehp;

  xercesc::DOMImplementation* impl;
  xml_schema::dom::unique_ptr<xercesc::DOMLSParser> parser;

  xercesc::DOMConfiguration* conf_r;

};
#pragma GCC diagnostic pop

#endif /* MAZENET_CLIENT_MESSAGEHANDLER_HPP */

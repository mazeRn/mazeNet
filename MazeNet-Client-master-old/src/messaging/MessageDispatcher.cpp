#include <util/logging/Log.hpp>
#include "MessageDispatcher.hpp"

const XMLCh ls_id[] = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" //Ignoring the warning.
MessageDispatcher::MessageDispatcher(TcpConnection::ConnectionPtr connection)
    : connection_(connection),
      eh(),
      ehp(eh),
      impl(xercesc::DOMImplementationRegistry::getDOMImplementation(ls_id)),
      writer(impl->createLSSerializer()),
      conf_w(writer->getDomConfig()){

  conf_w->setParameter(xercesc::XMLUni::fgDOMErrorHandler, &ehp);
  conf_w->setParameter(xercesc::XMLUni::fgDOMXMLDeclaration, false);


  out =
      xml_schema::dom::unique_ptr<xercesc::DOMLSOutput>(impl->createLSOutput());
  out->setByteStream(&ft);

  map_[""].name = "";
  map_[""].schema = "MazeCom.xsd";
}

void MessageDispatcher::send(const MazeCom& msg) {
  xml_schema::dom::unique_ptr<xercesc::DOMDocument> doc(
      MazeCom_(msg, map_));

  ft.reset();

  writer->write(doc.get(), out.get());
  eh.throw_if_failed<xml_schema::serialization>();

  connection_->send((const char*)(ft.getRawBuffer()));
}

void MessageDispatcher::sendLoginMessage(const std::string& name) {
  mazenet::util::logging::Log logger("network");
  MazeCom login_message(MazeComType(MazeComType::LOGIN), 0);
  login_message.LoginMessage(LoginMessageType(name));

  send(login_message);

  // logger.logSeverity(mazenet::util::logging::SeverityLevel::trace) << ss.str() << logger.end();
}

void MessageDispatcher::sendMove(int player_id, const Move& move) {
  mazenet::util::logging::Log logger("network");

  MazeCom mazecom_message(MazeComType::MOVE, player_id);
  positionType shift_position(move.shift_pos.row, move.shift_pos.col);
  positionType new_pin_position(move.new_pos.row, move.new_pos.col);

  openings open(move.shift_card.isOpen(Card::UP), move.shift_card.isOpen(Card::DOWN),
                move.shift_card.isOpen(Card::LEFT), move.shift_card.isOpen(Card::RIGHT));

  pin pins;
  for (int i = 1; i < 4; ++i) {
    if (move.shift_card.hasPlayer(i))
      pins.playerID().emplace_back(i);
  }

  cardType shift_card(open, pins);

  if (move.shift_card.getTreasure() != -1) {
    shift_card.treasure().set((treasureType::value) move.shift_card.getTreasure());
  }

  MoveMessageType move_message(shift_position, new_pin_position, shift_card);
  mazecom_message.MoveMessage(move_message);

  send(mazecom_message);

  // logger.logSeverity(mazenet::util::logging::SeverityLevel::trace) << ss.str() << logger.end();
}
#pragma GCC diagnostic pop

#ifndef MAZENET_CLIENT_MESSAGEDISPATCHER_HPP
#define MAZENET_CLIENT_MESSAGEDISPATCHER_HPP


#include <network/TcpConnection.hpp>

#include "mazeCom.hxx"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>

#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include <xercesc/util/XMLUniDefs.hpp>

#include <xsd/cxx/xml/dom/bits/error-handler-proxy.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

#include "../player/Move.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++" //Ignoring the warning.
class MessageDispatcher : public boost::noncopyable {
 public:
  MessageDispatcher(TcpConnection::ConnectionPtr connection);

  void sendLoginMessage(const std::string& name);

  void sendMove(int player_id, const Move& move);

  void send(const MazeCom& msg);
 private:
  TcpConnection::ConnectionPtr connection_;

  xsd::cxx::tree::error_handler<char> eh;
  xsd::cxx::xml::dom::bits::error_handler_proxy<char> ehp;
  xercesc::DOMImplementation* impl;
  xml_schema::dom::unique_ptr<xercesc::DOMLSSerializer> writer;
  xml_schema::dom::unique_ptr<xercesc::DOMLSOutput> out;
  xml_schema::namespace_infomap map_;
  xercesc::DOMConfiguration* conf_w;

  xercesc::MemBufFormatTarget ft;
};
#pragma GCC diagnostic pop

#endif //MAZENET_CLIENT_MESSAGEDISPATCHER_HPP

#include "TcpServer.hpp"

#include <iostream>
#include <boost/bind.hpp>

TcpServer::TcpServer() : io_service_(), acceptor_(io_service_) {

}

void TcpServer::start(uint32_t port) {
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  startAccept();

  std::cout << "Server listening on TCP-Port " << port << std::endl;
}

void TcpServer::stop() {
  // stop accepting connections and stop the io service
  acceptor_.close();
}

void TcpServer::setConnectionHandler( ConnectionHandler handler ) {
  handle_new_connection_ = handler;
}

void TcpServer::startAccept() {
  TcpConnection::ConnectionPtr newConnection = TcpConnection::createConnection(io_service_);
  // wait for new connection
  acceptor_.async_accept(newConnection->getSocket(), boost::bind (&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
}

void TcpServer::handleAccept(TcpConnection::ConnectionPtr connection, const boost::system::error_code& error) {
  if(!error) {
    connection->start();
    std::cout << "New Connection from client: " << connection->getRemoteAddress() <<":" << connection->getRemotePort() << std::endl;
    handle_new_connection_(connection);
  }
  // wait for more connections
  startAccept();
}

boost::asio::io_service& TcpServer::getIOService() {
  return io_service_;
}

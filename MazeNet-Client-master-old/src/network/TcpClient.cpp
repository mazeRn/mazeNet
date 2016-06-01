#include "TcpClient.hpp"

#include <boost/asio/ip/tcp.hpp>

TcpClient::TcpClient()
    : io_service_(),
      connection_(TcpConnection::createConnection(io_service_)) {}

void TcpClient::openConnection(const std::string& host,
                               const std::string& port) {
  boost::asio::ip::tcp::resolver resolver(io_service_);

  // resolve the host and port and start connecting
  connection_->getSocket().connect(
      *resolver.resolve(boost::asio::ip::tcp::resolver::query(host, port)));
  connection_->start();
}

void TcpClient::closeConnection() {
  connection_->close();
}

boost::asio::io_service& TcpClient::getIOService() { return io_service_; }

TcpConnection::ConnectionPtr TcpClient::getConnection() { return connection_; }
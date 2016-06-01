#include "TcpConnection.hpp"

#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/read_at.hpp>
#include <memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

TcpConnection::TcpConnection(boost::asio::io_service& io_service)
    : socket_(io_service), remoteAddress_(), readMessage_(), writeMessage_() {
  auto uuid = boost::uuids::random_generator()();
  uuid_ = boost::lexical_cast<std::string>(uuid);
}

TcpConnection::ConnectionPtr TcpConnection::createConnection(boost::asio::io_service& io_service) {
  return ConnectionPtr(new TcpConnection(io_service));
}

TcpConnection::TcpSocket& TcpConnection::getSocket() {
  return socket_;
}

boost::asio::ip::address TcpConnection::getRemoteAddress() {
  return remoteAddress_;
}

uint32_t TcpConnection::getRemotePort() {
  return remotePort_;
}

void TcpConnection::start() {
  if (!connected_) {
    connected_ = true;
    remoteAddress_ = socket_.remote_endpoint().address();
    remotePort_ = socket_.remote_endpoint().port();
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(&msgLenNetOrder_, TcpMessage::header_len_),
        boost::bind(&TcpConnection::handle_read_head, this,
                    boost::asio::placeholders::error));
  }
}

void TcpConnection::close() {
  if (connected_) {
    connected_ = false;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    socket_.close();
  }
}

void TcpConnection::send(const std::string& msg) {
  bool write_in_progress = !writeMessage_.empty();
  writeMessage_.push_back(TcpMessage(msg));
  if (!write_in_progress) {
    boost::asio::async_write(socket_, boost::asio::buffer(writeMessage_.front().getData(),
                                                          writeMessage_.front().getDataLength()),
                             boost::bind(&TcpConnection::handle_write, this, boost::asio::placeholders::error));
  }
}

void TcpConnection::handle_read_head(const boost::system::error_code& ec) {
  if (!ec && readMessage_.setBodyLengthNO(msgLenNetOrder_)) {
    boost::asio::async_read(
        socket_, boost::asio::buffer(readMessage_.getBody(),
                                     readMessage_.getBodyLength()),
        boost::bind(&TcpConnection::handle_read_body, this, boost::asio::placeholders::error));
  } else {
    handle_error(ec);
  }
}

void TcpConnection::handle_read_body(const boost::system::error_code& ec) {
  if (!ec) {
    on_read_message_(readMessage_.getMessage());
    boost::asio::async_read(socket_,
                            boost::asio::buffer(&msgLenNetOrder_,
                                                TcpMessage::header_len_),
                            boost::bind(&TcpConnection::handle_read_head, this, boost::asio::placeholders::error));
  } else {
    handle_error(ec);
  }
}

void TcpConnection::handle_write(const boost::system::error_code& ec) {
  if (!ec) {
    on_send_complete_();
    writeMessage_.pop_front();
    if (!writeMessage_.empty()) {
      boost::asio::async_write(
          socket_, boost::asio::buffer(writeMessage_.front().getData(),
                                       writeMessage_.front().getDataLength()),
          boost::asio::transfer_all(),
          boost::bind(&TcpConnection::handle_write, this, boost::asio::placeholders::error));
    }
  } else {
    handle_error(ec);
  }
}

void TcpConnection::handle_error(const boost::system::error_code& ec) {
  // this part is very tricky, this we want to allow the handler to
  // free this connection
  // The connected flag will only allow one handler to enter
  if (connected_) {
    connected_ = false;

    // this will abort all handlers
    socket_.cancel();
    socket_.close();

    // now we can handle and the error (and may also free it)
    error_handler_(ec);
  }
}

void TcpConnection::setErrorHandler(ErrorHandler handler) {
  error_handler_ = handler;
}

void TcpConnection::setReadHandler(MessageReceiveHandler handler) {
  on_read_message_ = handler;
}

void TcpConnection::setWriteHandler(SendCompleteHandler handler) {
  on_send_complete_ = handler;
}

const std::string TcpConnection::getUUID() const {
  return uuid_;
}

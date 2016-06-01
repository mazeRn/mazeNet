#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include "TcpConnection.hpp"

#include <memory>


/**
 * the TcpServer listens a port and starts accepting connections
 */
class TcpServer : public boost::noncopyable {
 public:
  using ConnectionHandler = std::function<void(TcpConnection::ConnectionPtr)>;

  /**
   * default constructor
   */
  TcpServer();

  /**
   * default destructor
   */
  virtual ~TcpServer() = default;

  /**
   * start listening on the port
   */
  void start(uint32_t port);

  /**
   * stop listening on the port
   */
  void stop();

  /**
   * set a handler for new connections
   */
  void setConnectionHandler(ConnectionHandler handler);

  boost::asio::io_service& getIOService();
 private:
  /**
   * sets a async handler for new connections
   */
  void startAccept();

  /**
   * handles a new connection
   */
  void handleAccept(TcpConnection::ConnectionPtr connection,
                    const boost::system::error_code& error);

  /**
   * internal io_service
   */
  boost::asio::io_service io_service_;

  /**
   * acceptor for new connections
   */
  boost::asio::ip::tcp::acceptor acceptor_;

  /**
   * handler for new connections
   */
  ConnectionHandler handle_new_connection_ = [](TcpConnection::ConnectionPtr){};
};

#endif /* TCPSERVER_HPP */

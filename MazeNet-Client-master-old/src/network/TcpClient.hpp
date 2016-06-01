#ifndef _TCPCLIENT_HPP_
#define _TCPCLIENT_HPP_

#include "TcpConnection.hpp"


/**
 * the TcpClient classes manages a connection to a remote host.
 */
class TcpClient : public boost::noncopyable {
 public:
  /**
   * default constructor
   */
  TcpClient();

  /**
   * opens a new connection a starts the io service
   */
  void openConnection(const std::string& host, const std::string& port);

  /**
   * returns the ConnectionPtr
   */
  TcpConnection::ConnectionPtr getConnection();

  /**
   * returns a reference to the io service
   */
  boost::asio::io_service& getIOService();

  /**
   * closes the connection and stops the io service
   */
  void closeConnection();
 private:
  /**
   * internal io service for executing the async operations
   */
  boost::asio::io_service io_service_;

  /**
   * internal tcp connection
   */
  TcpConnection::ConnectionPtr connection_;
};

#endif /* _TCPCLIENT_HPP_ */

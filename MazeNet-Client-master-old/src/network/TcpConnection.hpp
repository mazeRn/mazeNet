#ifndef TCPCONNECTION_HPP
#define TCPCONNECTION_HPP

#include "TcpMessage.hpp"

#include <memory>
#include <functional>
#include <deque>

#include <boost/asio.hpp>

/**
 * manages tcp connections and provides methods for setting several
 * eventhandlers
 */
class TcpConnection : boost::noncopyable {
 public:
  using ConnectionPtr = std::shared_ptr<TcpConnection>;
  using TcpSocket = boost::asio::ip::tcp::socket;
  using ErrorHandler = std::function<void(const boost::system::error_code&)>;
  using MessageReceiveHandler = std::function<void(std::string)>;
  using SendCompleteHandler = std::function<void(void)>;

  /**
   * factory method for creating a new connection
   */
  static ConnectionPtr createConnection(boost::asio::io_service& io_service);

  /**
   * starts the connection
   */
  void start();

  /**
   * closes the connection
   */
  void close();

  /**
   * sends a message to the remote host
   */
  void send(const std::string& msg);

  /**
   * returns a reference to the internal socket
   */
  TcpSocket& getSocket();

  /**
   * returns the remote address
   */
  boost::asio::ip::address getRemoteAddress();

  /**
   * returns the remote port
   */
  uint32_t getRemotePort();

  /**
   * sets a handler that is called if errors occur
   */
  void setErrorHandler(ErrorHandler handler);
  /**
   * sets a handler that is called when a write operation
   * has finished.
   */
  void setWriteHandler(SendCompleteHandler handler);

  /**
   * sets a handler that is called on receiving a new message
   */
  void setReadHandler(MessageReceiveHandler handler);

  /**
   * default destructor
   */
  virtual ~TcpConnection() = default;

  const std::string getUUID() const;

 private:
  /**
   * private constructor since we use a factory method
   */
  TcpConnection(boost::asio::io_service& io_service);

  /**
   * handles completed write operations
   */
  void handle_write(const boost::system::error_code& ec);

  /**
   * called after the message header has been read
   */
  void handle_read_head(const boost::system::error_code& ec);

  /**
   * called after the message body has been read
   */
  void handle_read_body(const boost::system::error_code& ec);

  /**
   * called on error
   */
  void handle_error(const boost::system::error_code& ec);

  /**
   * internal tcp socket
   */
  TcpSocket socket_;

  /**
   * address of the remote host
   */
  boost::asio::ip::address remoteAddress_;

  /**
   * port of the remote host
   */
  uint32_t remotePort_ = -1;

  /**
   * internal connection state
   */
  bool connected_ = false;

  /**
   * message length in network order for temporary storage
   */
  TcpMessage::header_len_t msgLenNetOrder_ = -1;

  /**
   * message read from the server
   */
  TcpMessage readMessage_;

  /**
   * messages to send to the server
   */
  std::deque<TcpMessage> writeMessage_;

  /**
   * external handler for read messages
   */
  MessageReceiveHandler on_read_message_ = [](std::string){};

  /**
   * external send handler
   */
  SendCompleteHandler on_send_complete_ = []{};

  /**
   * external error handler
   */
  ErrorHandler error_handler_ = [](const boost::system::error_code&){};

  std::string uuid_ = "";
};

#endif /* TCPCONNECTION_HPP */

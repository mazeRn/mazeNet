#ifndef TCPMESSAGE_HPP
#define TCPMESSAGE_HPP

#include <string>
#include <cstdlib>
#include <memory>

/**
 * The TcpMessage wraps messages for tcp communication.
 * It consists of a 2 or 4 Byte length header and a body of that length
 * It performs host byteorder to network byteorder conversions.
 */
class TcpMessage {
 public:
// this switch will determine if 2 or 4 bytes are used for length encoding
#ifdef TWO_BYTE_TCP_LENGTH
  /**
   * Length of the message header.
   * use 2 for java compatible short messages
   * and 4 for long TCP messages.
   * Server and Client are always expected to use the same length.
   * Warning: This is NOT checked and WILL break the communication.
   */
  constexpr static const uint32_t header_len_ = 2;
  using header_len_t = uint16_t;

  /**
   * Maximum body length
   */
  constexpr static const uint16_t max_body_len_ = INT16_MAX;
#else   // _2_BYTE_LEN_HEADER
  constexpr static const uint32_t header_len_ = 4;
  using header_len_t = uint32_t;
  constexpr static const uint32_t max_body_len_ = INT32_MAX;
#endif  // _2_BYTE_LEN_HEADER

  /**
   * default constructor will create an empty message
   */
  TcpMessage();

  /**
   * this will wrap a string into the message
   */
  TcpMessage(const std::string& msg);

  /**
   * copy construct a message
   */
  TcpMessage(const TcpMessage& orig);

  /**
   * move construct a message
   */
  TcpMessage(TcpMessage&& orig);

  /**
   * copy-assignment
   */
  TcpMessage& operator=(const TcpMessage& orig);

  /**
   * move-assignment
   */
  TcpMessage& operator=(TcpMessage&& orig);

  /**
   * default destructor
   */
  virtual ~TcpMessage() = default;

  /**
   * obtain the stored data. This includes the header
   * @returns pointer to internal data.
   */
  const char* getData() const;

  /**
   * obtain the stored data. This includes the header
   * @returns pointer to the internal data
   */
  char* getData();

  /**
   * returns the internal data length, including the header,
   * in host order
   */
  header_len_t getDataLength() const;

  /**
   * returns the stored data without header
   */
  char* getBody();

  /**
   * returns the length of the stored data without the header
   * in host order
   */
  header_len_t getBodyLength() const;

  /**
   * returns the stored message as a string
   */
  std::string getMessage() const;

  /**
   * sets the body length in network order
   */
  bool setBodyLengthNO(header_len_t len_in_network_order_);

  /**
   * body length in host order
   */
  header_len_t body_len_host_order_ = 0;

 private:
  /**
   * stored data
   */
  std::unique_ptr<char[]> data_;
};

#endif /* TCPMESSAGE_HPP */

#include "TcpMessage.hpp"

#include <sstream>
#include <netinet/in.h>

TcpMessage::TcpMessage() : data_() {

}

TcpMessage::TcpMessage(const TcpMessage& orig)
    : body_len_host_order_(orig.body_len_host_order_), data_() {
  // copy data
  data_ = std::unique_ptr<char[]>(new char[body_len_host_order_ + header_len_]);
  std::copy(orig.data_.get(),
            orig.data_.get() + body_len_host_order_ + header_len_, data_.get());
}

TcpMessage::TcpMessage(const std::string& message) : data_() {
  const char* msgArray = message.c_str();
  body_len_host_order_ = message.length();

// perform host to network order conversion
#ifdef TWO_BYTE_TCP_LENGTH
  header_len_t body_len_net_order = htons(body_len_host_order_);
#else
  header_len_t body_len_net_order = htonl(body_len_host_order_);
#endif

  // copy data
  data_ = std::unique_ptr<char[]>(new char[body_len_host_order_ + header_len_]);
  std::copy((char*) (&body_len_net_order),
            (char*) (&body_len_net_order) + header_len_, data_.get());
  std::copy(msgArray, msgArray + body_len_host_order_,
            data_.get() + header_len_);
}

TcpMessage::TcpMessage(TcpMessage&& orig)
    : body_len_host_order_(orig.body_len_host_order_), data_(std::move(orig.data_)) {
}

TcpMessage& TcpMessage::operator=(const TcpMessage& orig) {
  if (this != &orig) {
    body_len_host_order_ = orig.body_len_host_order_;
    auto temp = std::unique_ptr<char[]>(new char[body_len_host_order_ + header_len_]);
    std::copy(orig.data_.get(),
              orig.data_.get() + body_len_host_order_ + header_len_,
              temp.get());
    std::swap(temp, data_);
  }
  return *this;
}

TcpMessage& TcpMessage::operator=(TcpMessage&& orig) {
  if (this != &orig) {
    auto temp =
        std::unique_ptr<char[]>(new char[body_len_host_order_ + header_len_]);
    std::copy(orig.data_.get(),
              orig.data_.get() + body_len_host_order_ + header_len_,
              temp.get());
    std::swap(temp, data_);

    body_len_host_order_ = orig.body_len_host_order_;
  }
  return *this;
}

char* TcpMessage::getData() { return data_.get(); }

TcpMessage::header_len_t TcpMessage::getDataLength() const {
  return body_len_host_order_ + header_len_;
}

char* TcpMessage::getBody() { return data_.get() + header_len_; }

TcpMessage::header_len_t TcpMessage::getBodyLength() const {
  return body_len_host_order_;
}

bool TcpMessage::setBodyLengthNO(header_len_t len_in_network_order_) {
#ifdef TWO_BYTE_TCP_LENGTH
  body_len_host_order_ = ntohs(len_in_network_order_);
#else
  body_len_host_order_ = ntohl(len_in_network_order_);
#endif

  if (body_len_host_order_ > max_body_len_) {
    body_len_host_order_ = 0;
    data_ = nullptr;
    return false;
  } else {
    data_ =
        std::unique_ptr<char[]>(new char[body_len_host_order_ + header_len_]);
    char* ptrToLength = (char*) &len_in_network_order_;
    for (uint8_t i = 0; i < 4; ++i, ++ptrToLength) {
      data_[i] = *ptrToLength;
    }
  }
  return true;
}

std::string TcpMessage::getMessage() const {
  if (body_len_host_order_) {
    std::stringstream ss;
    ss.write(data_.get() + header_len_, body_len_host_order_);
    return ss.str();
  } else {
    return std::string("");
  }
}

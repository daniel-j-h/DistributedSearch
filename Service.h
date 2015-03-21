#ifndef SEARCH_SERVICE_H
#define SEARCH_SERVICE_H

#include <cassert>
#include <string>
#include <stdexcept>

#include <nanomsg/nn.h>
#include <nanomsg/survey.h>

#define throw_on(...) \
  if (__VA_ARGS__) throw std::runtime_error{::nn_strerror(::nn_errno())};

namespace Service {

class Requester final {
public:
  // TODO: setsockopt deadline, default 1s
  Requester(const std::string& address) {
    sock_ = ::nn_socket(AF_SP, NN_SURVEYOR);
    throw_on(sock_ < 0);

    auto rv = ::nn_bind(sock_, address.c_str());
    throw_on(rv < 0);
  }

  ~Requester() {
    auto rv = ::nn_close(sock_);
    assert(rv == 0);
  }

private:
  int sock_;
};


class Responder final {
public:
  Responder(const std::string& endpoint) {
    sock_ = ::nn_socket(AF_SP, NN_RESPONDENT);
    throw_on(sock_ < 0);

    auto rv = ::nn_connect(sock_, endpoint.c_str());
    throw_on(rv < 0);
  }

  ~Responder() {
    auto rv = ::nn_close(sock_);
    assert(rv == 0);
  }

private:
  int sock_;
};
}

#undef throw_on

#endif

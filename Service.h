#ifndef SEARCH_SERVICE_H
#define SEARCH_SERVICE_H

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <memory>
#include <iterator>

#include <nanomsg/nn.h>
#include <nanomsg/survey.h>

#include "Messages_reflection.h"
#include <bond/core/bond.h>
#include <bond/stream/output_buffer.h>

#include "Common.h"

#define nn_throw_on(...)                                                                                               \
  if (__VA_ARGS__)                                                                                                     \
    throw std::runtime_error{::nn_strerror(::nn_errno())};

namespace Service {

class Requester final {
public:
  Requester(const std::string& address, int timeout = 1000) {
    sock_ = ::nn_socket(AF_SP, NN_SURVEYOR);
    nn_throw_on(sock_ < 0);

    int rv;

    rv = ::nn_setsockopt(sock_, NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &timeout, sizeof(timeout));
    nn_throw_on(rv < 0);

    rv = ::nn_bind(sock_, address.c_str());
    nn_throw_on(rv < 0);
  }

  ~Requester() {
    const int rv = ::nn_close(sock_);
    assert(rv == 0);
  }

  Common::Response query(const std::string& keyword) {
    // make request
    Message::Request req;
    req.keyword = keyword;

    bond::OutputBuffer output;
    bond::CompactBinaryWriter<bond::OutputBuffer> writer{output};
    Serialize(req, writer);

    const auto blob = output.GetBuffer();

    int rv;
    rv = ::nn_send(sock_, blob.data(), blob.size(), 0);
    nn_throw_on(rv < 0);

    // gather responses
    char* recv_buffer{};
    const auto releaser = [](void* msg) { ::nn_freemsg(msg); };

    // merge responses into one, containing all matches
    Common::Response matches;

    // Timeout is currently signaled by errno == EFSM instead of ETIMEDOUT, see these issues:
    // * http://www.freelists.org/post/nanomsg/Expired-nn-survey-deadline-error-mismatch
    // * https://github.com/nanomsg/nanomsg/issues/194
    while ((rv = ::nn_recv(sock_, &recv_buffer, NN_MSG, 0)) >= 0) {
      assert(recv_buffer);

      // make sure the receive buffer is cleaned up on all exit paths (e.g. bond may throw)
      const std::unique_ptr<char[], decltype(releaser)> defer{recv_buffer, releaser};

      bond::InputBuffer input{recv_buffer, static_cast<std::uint32_t>(rv)};
      bond::CompactBinaryReader<bond::InputBuffer> reader{input};

      Message::Response resp;
      Deserialize(reader, resp);

      // this merges all responses, discarding duplicates (set property)
      matches.insert(begin(resp.matches), end(resp.matches));
    }

    return matches;
  }

  Requester(const Requester&) = delete;
  Requester& operator=(const Requester&) = delete;

private:
  int sock_;
};


class Responder final {
public:
  Responder(const std::string& endpoint) {
    sock_ = ::nn_socket(AF_SP, NN_RESPONDENT);
    nn_throw_on(sock_ < 0);

    const int rv = ::nn_connect(sock_, endpoint.c_str());
    nn_throw_on(rv < 0);
  }

  ~Responder() {
    const int rv = ::nn_close(sock_);
    assert(rv == 0);
  }

  template <typename RequestHandler>
  void onRequest(RequestHandler handler) {
    int rv;
    char* recv_buffer{};
    const auto releaser = [](void* msg) { ::nn_freemsg(msg); };

    // eventloop
    for (;;) {
      // get keyword request
      rv = ::nn_recv(sock_, &recv_buffer, NN_MSG, 0);
      nn_throw_on(rv < 0);

      assert(recv_buffer);

      // make sure the receive buffer is cleaned up on all exit paths (e.g. bond may throw)
      const std::unique_ptr<char[], decltype(releaser)> defer{recv_buffer, releaser};

      bond::InputBuffer input{recv_buffer, static_cast<std::uint32_t>(rv)};
      bond::CompactBinaryReader<bond::InputBuffer> reader{input};

      Message::Request req;
      Deserialize(reader, req);

      // respond with matches; call site provides handler
      const Common::Response matches = handler(Common::Request{req.keyword});

      Message::Response resp;
      resp.matches.insert(begin(matches), end(matches));

      bond::OutputBuffer output;
      bond::CompactBinaryWriter<bond::OutputBuffer> writer{output};
      Serialize(resp, writer);

      const auto blob = output.GetBuffer();

      rv = ::nn_send(sock_, blob.data(), blob.size(), 0);
      nn_throw_on(rv < 0);
    }
  }

  Responder(const Responder&) = delete;
  Responder& operator=(const Responder&) = delete;

private:
  int sock_;
};
}

#undef nn_throw_on

#endif

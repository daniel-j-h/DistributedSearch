#include <cassert>
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <memory>
#include <iterator>
#include <utility>

#include <nanomsg/nn.h>
#include <nanomsg/survey.h>

#include "Messages_reflection.h"
#include <bond/core/bond.h>
#include <bond/stream/output_buffer.h>

#include <tbb/pipeline.h>

#include "Service.h"

#define nn_throw_on(...)                                                                                               \
  if (__VA_ARGS__)                                                                                                     \
    throw std::runtime_error{::nn_strerror(::nn_errno())};

namespace Service {

Requester::Requester(const std::string& address, int timeout) {
  sock_ = ::nn_socket(AF_SP, NN_SURVEYOR);
  nn_throw_on(sock_ < 0);

  int rv;

  rv = ::nn_setsockopt(sock_, NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &timeout, sizeof(timeout));
  nn_throw_on(rv < 0);

  rv = ::nn_bind(sock_, address.c_str());
  nn_throw_on(rv < 0);
}

Requester::~Requester() {
  const int rv = ::nn_close(sock_);
  assert(rv == 0);
}

Common::Response Requester::query(const std::string& keyword) {
  // make request
  Message::Request req;
  req.keyword = keyword;

  bond::OutputBuffer output;
  bond::CompactBinaryWriter<bond::OutputBuffer> writer{output};
  Serialize(req, writer);

  const auto blob = output.GetBuffer();

  int rv = ::nn_send(sock_, blob.data(), blob.size(), 0);
  nn_throw_on(rv < 0);

  // merge responses into one, containing all matches
  Common::Response matches;


  const auto releaser = [](void* msg) { ::nn_freemsg(msg); };
  // debian jessie's tbb version (4.2) does not support moving data between stages (yet?)
  // i.e. moveable unique_ptr -- why? tbb fix this! shared_ptr with custom deleter does the job.
  using Blob = std::shared_ptr<char>;
  using SizedBlob = std::pair<Blob, std::size_t>;

  // received memory blobs from the network and passes them on
  const auto receiveFn = [this, releaser](tbb::flow_control& flow) -> SizedBlob {
    int recvRv;
    char* recvBuffer{};

    if ((recvRv = ::nn_recv(sock_, &recvBuffer, NN_MSG, 0)) >= 0) {
      assert(recvBuffer);

      Blob memBlob{recvBuffer, releaser};
      SizedBlob sBlob{std::move(memBlob), static_cast<std::size_t>(recvRv)};
      return sBlob;
    } else {
      flow.stop();
      return SizedBlob{Blob{nullptr}, 0};
    }
  };

  // gets memory blobs from stage before, deserializes blobs as Responses and passes them on
  const auto deserializeFn = [](SizedBlob sBlob) -> Message::Response {
    bond::InputBuffer input{sBlob.first.get(), static_cast<std::uint32_t>(sBlob.second)};
    bond::CompactBinaryReader<bond::InputBuffer> reader{input};

    Message::Response resp;
    Deserialize(reader, resp);

    return resp;
    // blob's lifetime ends, releaser gets invoked, automatically cleaning up the message
  };

  // gets responses from stage before, merges them into single response object
  const auto mergeFn = [&](Message::Response resp) -> void {
    // this merges all responses, discarding duplicates (set property)
    matches.insert(begin(resp.matches), end(resp.matches));
  };


  // upper bound of number of stages that will be run concurrently
  const constexpr auto tokens = 256u;

  const constexpr auto par = tbb::filter::parallel;
  const constexpr auto seq = tbb::filter::serial_out_of_order;

  const auto receiveStage = tbb::make_filter<void, SizedBlob>(seq, receiveFn);
  const auto deserializeStage = tbb::make_filter<SizedBlob, Message::Response>(par, deserializeFn);
  const auto mergeStage = tbb::make_filter<Message::Response, void>(seq, mergeFn);

  tbb::parallel_pipeline(tokens, receiveStage & deserializeStage & mergeStage);

  return matches;
}

Responder::Responder(const std::string& endpoint) {
  sock_ = ::nn_socket(AF_SP, NN_RESPONDENT);
  nn_throw_on(sock_ < 0);

  const int rv = ::nn_connect(sock_, endpoint.c_str());
  nn_throw_on(rv < 0);
}

Responder::~Responder() {
  const int rv = ::nn_close(sock_);
  assert(rv == 0);
}

void Responder::onRequest(Common::RequestHandler handler) {
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

#undef nn_throw_on
}

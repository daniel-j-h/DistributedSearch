#include <iostream>

#include "Common.h"
#include "Service.h"

int main(int argc, char** argv) try {
  Common::init();

  const auto args = Common::args(argc, argv);
  const auto bind = args.size() >= 2 ? args[1] : "tcp://*:9996";
  const auto conn = args.size() == 3 ? args[2] : "tcp://localhost:9995";

  // requester represents attaching services
  Service::Requester requester{bind, 500};  // lower timeout, since we have to respond, too

  // responder represents service we send the results to
  Service::Responder responder{conn};

  // recursively query attached services; pass request on
  const auto recursiveRequestHandler = [&requester](const Common::Request& req) -> Common::Response {
    return requester.query(req);
  };

  responder.onRequest(recursiveRequestHandler);


} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

#include <iostream>

#include "Common.h"
#include "Service.h"

int main(int argc, char** argv) try {
  Common::init();

  const auto args = Common::args(argc, argv);
  const auto endpoint = args.size() == 2 ? args[1] : "tcp://localhost:9995";

  Service::Responder self{endpoint};

  // dummy request handler
  const auto requestHandler = [&self](const Common::Request& req) -> Common::Response {
    (void)req; // unused
    return {"First Web Result", "Second Web Result"};
  };

  self.onRequest(requestHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

#include <iostream>
#include <string>
#include <stdexcept>

#include "Common.h"
#include "Service.h"

int main(int /*argc*/, char** /*argv*/) try {
  Common::init();

  Service::Responder self{"tcp://localhost:9995"};

  // dummy request handler
  const auto requestHandler = [&self](const Common::Request& req) -> Common::Response {
    (void)req; // unused
    return {"First Web Result", "Second Web Result"};
  };

  self.onRequest(requestHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

#include <iostream>
#include <stdexcept>

#include "Common.h"
#include "Search.h"
#include "Service.h"

int main(int /*argc*/, char** /*argv*/) try {
  Common::init();

  Service::Requester self{"tcp://*:9995"};

  const auto requestHandler = [&self] (const Common::Request& req) -> Common::Response {
    return self.query(req);
  };

  Search::interact(requestHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

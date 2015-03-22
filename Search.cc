#include <iostream>
#include <stdexcept>

#include "Common.h"
#include "Search.h"
#include "Service.h"

int main(int /*argc*/, char** /*argv*/) try {
  Common::init();

  Service::Requester self{"tcp://*:9995"};

  const auto interactionHandler = [&self](const Common::Request& req) -> Common::Response {
    // query all attached services on interaction
    return self.query(req);
  };

  Search::onInteraction(interactionHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

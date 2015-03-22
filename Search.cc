#include <iostream>
#include <stdexcept>

#include "Common.h"
#include "Search.h"
#include "Service.h"

int main(int argc, char** argv) try {
  Common::init();

  const auto args = Common::args(argc, argv);
  const auto endpoint = args.size() == 2 ? args[1] : "tcp://*:9995";

  Service::Requester self{endpoint};

  const auto interactionHandler = [&self](const Common::Request& req) -> Common::Response {
    // query all attached services on interaction
    return self.query(req);
  };

  Search::onInteraction(interactionHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

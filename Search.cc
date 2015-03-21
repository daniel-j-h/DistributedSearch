#include <iostream>
#include <stdexcept>

#include "Config.h"
#include "Search.h"
#include "Service.h"

int main(int /*argc*/, char** /*argv*/) try {
  Config::init();

  Service::Requester self{"tcp://*:9995"};

  const auto requestHandler = [&self] (const Config::Request&/*req*/) -> Config::Response {
    return self.query(5/*ts*/,"Cats");
  };

  Search::interact(requestHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

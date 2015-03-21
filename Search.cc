#include <iostream>
#include <stdexcept>

#include "Config.h"
#include "Search.h"
#include "Service.h"

using Request = std::string;
using Response = std::vector<std::string>;

int main(int /*argc*/, char** /*argv*/) try {
  Config::init();

  Service::Requester self{"tcp://*:9995"};

  const auto requestHandler = [] (const Request&/*req*/) -> Response {
    return {"First", "Second"};
  };

  Search::interact(requestHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

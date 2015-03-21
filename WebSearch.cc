#include <iostream>
#include <string>
#include <stdexcept>

#include "Common.h"
//#include "WebSearch.h"
#include "Service.h"

int main(int /*argc*/, char** /*argv*/) try {
  Common::init();

  Service::Responder self{"tcp://localhost:9995"};

  for (std::string line; std::getline(std::cin, line);)
    std::cout << "OK" << std::endl;// keepalive

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

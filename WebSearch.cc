#include <iostream>
#include <string>
#include <stdexcept>

#include "Common.h"
//#include "WebSearch.h"
#include "Service.h"

int main(int /*argc*/, char** /*argv*/) try {
  Common::init();

  Service::Responder self{"tcp://localhost:9995"};

  const auto requestHandler = [&self] (const Common::Request& req) -> Common::Response {
    return {"Fst", "Snd"};
  };

  self.onRequest(requestHandler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

#include <iostream>
#include <string>

#include "Common.h"
#include "Service.h"


void onInteraction(Common::RequestHandler);


int main(int argc, char** argv) try {
  Common::init();

  const auto args = Common::args(argc, argv);
  const auto endpoint = args.size() == 2 ? args[1] : "tcp://*:9995";

  Service::Requester self{endpoint};

  const auto handler = [&self](const Common::Request& req) -> Common::Response {
    // query all attached services on interaction
    return self.query(req);
  };

  onInteraction(handler);

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}


void onInteraction(Common::RequestHandler handler) {
  std::string request;
  do {
    if (!request.empty()) {
      const auto responses = handler(request);

      std::cout << "\nResults>\n";
      for (auto&& resp : responses)
        std::cout << " * " << resp << '\n';
    }
    std::cout << "\nSearch>" << std::endl;
  } while (std::getline(std::cin, request));
}

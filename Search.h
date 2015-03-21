#ifndef SEARCH_SEARCH_H
#define SEARCH_SEARCH_H

#include <iostream>
#include <string>

namespace Search {
template <typename RequestHandler>
void interact(RequestHandler handler) {
  std::string request;
  do {
    if (not request.empty()) {
      const auto responses = handler(request);

      std::cout << "\nResults>\n";
      for (auto&& resp : responses)
        std::cout << " * " << resp << '\n';
    }
    std::cout << "\nSearch>" << std::endl;
  } while (std::getline(std::cin, request));
}
}

#endif

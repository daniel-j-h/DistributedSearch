#include <locale>
#include <iostream>
#include <ios>
#include <string>
#include <iterator>
#include <stdexcept>

int main(int /*argc*/, char** /*argv*/) try {
  std::locale::global(std::locale(""));
  std::ios_base::sync_with_stdio(false);


  std::string request;

  do {
    if (not request.empty()) {
      // TODO: make service requests, gather and merge
      std::cout << "\nResults:\n*\n*\n" << std::endl;
    }

    std::cout << "Search:" << std::endl;
  } while (std::getline(std::cin, request));

} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}

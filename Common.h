#ifndef SEARCH_COMMON_H
#define SEARCH_COMMON_H

#include <ios>
#include <locale>
#include <vector>
#include <string>
#include <set>

namespace Common {

using Request = std::string;
using Response = std::set<std::string>;

void init() noexcept {
  std::locale::global(std::locale(""));
  std::ios_base::sync_with_stdio(false);
}

std::vector<std::string> args(int argc, char** argv) {
  return {argv, argv + argc};
}

}

#endif

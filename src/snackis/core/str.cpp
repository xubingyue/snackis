#include "snackis/core/str.hpp"

namespace snackis {
  const str whitespace(" \t\r\n");
  
  str trim(const str& in) {
    const size_t i = in.find_first_not_of(whitespace);
    if (i == str::npos) { return ""; }
    const size_t j = in.find_last_not_of(whitespace);
    return in.substr(i, (j-i+1));
  }
}

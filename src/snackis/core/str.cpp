#include "snackis/core/str.hpp"

namespace snackis {
  const str whitespace(" \t\r\n");
  
  str trim(const str& in) {
    const size_t i = in.find_first_not_of(whitespace);
    if (i == str::npos) { return ""; }
    const size_t j = in.find_last_not_of(whitespace);
    return in.substr(i, (j-i+1));
  }

  int64_t to_int64(const str &in) {
    return std::stoll(in);
  }

  str bin_hex(const char *in, size_t len) {
    //TODO: convert using sodium
  }
  
  Data hex_bin(const str &in) {
    //TODO: convert using sodium
  }
}

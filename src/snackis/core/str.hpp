#ifndef SNACKIS_STR_HPP
#define SNACKIS_STR_HPP

#include <string>
#include "snackis/core/data.hpp"
#include "snackis/core/opt.hpp"

namespace snackis {
  using str = std::string;
  extern const str whitespace;

  str trim(const str& in);
  str fill(const str &in, char ch, size_t len);
  opt<int64_t> to_int64(const str &in);

  template <typename T>
  str to_str(const T &in) {
    return std::to_string(in);
  }

  str bin_hex(const unsigned char *in, size_t len);
  Data hex_bin(const str &in);

  size_t prefix_len(str x, str y);
}

#endif

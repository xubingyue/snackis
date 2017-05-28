#ifndef SNACKIS_STR_HPP
#define SNACKIS_STR_HPP

#include <string>

namespace snackis {
  using str = std::string;
  extern const str whitespace;

  str trim(const str& in);
  int64_t to_int64(const str &in);

  template <typename T>
  str to_str(const T &in) {
    return std::to_string(in);
  }
}

#endif

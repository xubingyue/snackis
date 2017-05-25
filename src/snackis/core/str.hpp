#ifndef SNACKIS_STR_HPP
#define SNACKIS_STR_HPP

#include <string>

namespace snackis {
  using str = std::string;
  extern const str whitespace;
  str trim(const str& in);
}

#endif

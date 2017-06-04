#ifndef SNACKIS_FORMAT_HPP
#define SNACKIS_FORMAT_HPP

#include <fmt/format.h>
#include "snackis/core/str.hpp"

namespace snackis {
  template <typename...Args>
  str format(const str& spec, Args...args) {
    return fmt::format(spec, std::forward<Args>(args)...);
  }
}

#endif

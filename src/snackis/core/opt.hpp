#ifndef SNACKIS_OPT_HPP
#define SNACKIS_OPT_HPP

#include <optional>

namespace snackis {
  template <typename T>
  using opt = std::optional<T>;

  extern const std::nullopt_t nullopt;
}

#endif

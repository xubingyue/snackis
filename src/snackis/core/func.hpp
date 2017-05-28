#ifndef SNACKIS_FUNC_HPP
#define SNACKIS_FUNC_HPP

#include <functional>

namespace snackis {
  template <typename T>
  using func = std::function<T>;
}

#endif

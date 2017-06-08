#ifndef SNACKIS_TUPLE_HPP
#define SNACKIS_TUPLE_HPP

#include <tuple>

namespace snackis {
  template <typename... Args, typename Func>
  void for_each(const std::tuple<Args...>& t, const Func& f) {
    std::apply([&f](const auto&... args) { (f(args), ...); }, t);
  }
}

#endif

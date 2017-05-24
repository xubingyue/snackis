#ifndef SNACKIS_OPT_HPP
#define SNACKIS_OPT_HPP

#include <boost/optional.hpp>

namespace snackis {
  template <typename T>
  using opt = boost::optional<T>;

  extern const boost::none_t none;
}

#endif

#ifndef SNACKIS_OPTIONAL_HPP
#define SNACKIS_OPTIONAL_HPP

#include <boost/optional.hpp>

namespace snackis {
  template <typename T>
  using optional = boost::optional<T>;

  extern const boost::none_t none;
}

#endif

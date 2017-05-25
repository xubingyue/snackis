#ifndef SNACKIS_OPT_HPP
#define SNACKIS_OPT_HPP

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

namespace snackis {
  template <typename T>
  using opt = boost::optional<T>;

  extern const boost::none_t none;
}

#endif

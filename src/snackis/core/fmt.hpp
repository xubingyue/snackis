#ifndef SNACKIS_FMT_HPP
#define SNACKIS_FMT_HPP

#include <boost/format.hpp>

#include "snackis/core/str.hpp"

namespace snackis {
  struct fmt: public boost::format {
    fmt(const snackis::str &spec);
    operator snackis::str() const;
    operator const char *() const;
    template<typename T>
    fmt &operator %(const T &arg);
  };

  template<typename T>
  fmt &fmt::operator %(const T &arg) {
    boost::format::operator %(arg);
    return *this;
  }
}

#endif

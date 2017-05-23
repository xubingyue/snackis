#ifndef SNACKIS_FMT_HPP
#define SNACKIS_FMT_HPP

#include <boost/format.hpp>

namespace snackis {
  struct fmt: public boost::format {
    fmt(const std::string &spec);
    operator std::string();
    operator const char *();
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

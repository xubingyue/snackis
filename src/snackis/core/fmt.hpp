#ifndef SNACKIS_FMT_HPP
#define SNACKIS_FMT_HPP

#include <boost/format.hpp>

namespace snackis {
  using Fmt = boost::format;
  
  Fmt fmt(const std::string spec);
}

#endif

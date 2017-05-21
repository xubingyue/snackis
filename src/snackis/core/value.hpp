#ifndef SNACKIS_VALUE_HPP
#define SNACKIS_VALUE_HPP

#include <boost/variant.hpp>
#include <cstdint>
#include <string>

#include "snackis/core/uid.hpp"

namespace snackis {
  using namespace boost;
  using Value = variant<int64_t, string, UId>;
}

#endif

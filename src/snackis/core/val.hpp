#ifndef SNACKIS_VAL_HPP
#define SNACKIS_VAL_HPP

#include <boost/variant.hpp>
#include <cstdint>
#include <string>

#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  using Val = boost::variant<int64_t, std::string, Time, UId>;
}

#endif

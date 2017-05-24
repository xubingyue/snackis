#ifndef SNACKIS_VAL_HPP
#define SNACKIS_VAL_HPP

#include <boost/variant.hpp>
#include <cstdint>
#include <string>

#include "snackis/core/data.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/key.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  using Val = boost::variant<int64_t, std::string, void *,
			     Data, Time, UId,
			     crypt::Key, crypt::PubKey>;

  template <typename T>
  T get(const Val &val) { return boost::get<T>(val); }
}

#endif

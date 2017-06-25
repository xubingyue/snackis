#ifndef SNACKIS_VAL_HPP
#define SNACKIS_VAL_HPP

#include <variant>
#include <vector>
#include <cstdint>

#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/key.hpp"
#include "snackis/crypt/pub_key.hpp"

namespace snackis {
  using Val = std::variant<bool, str, Time, UId,
			   int8_t, int16_t, int32_t, int64_t,
			   crypt::Key, crypt::PubKey>;

  template <typename T>
  T get(const Val &val) { return std::get<T>(val); }
}

#endif

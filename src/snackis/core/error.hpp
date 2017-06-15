#ifndef SNACKIS_ERROR_HPP
#define SNACKIS_ERROR_HPP

#include <stdexcept>

#include "snackis/core/str.hpp"
#include "snackis/core/macros.hpp"
#include "snackis/core/trace.hpp"

#define ERROR(type, msg) do {					\
    TRACE("Error thrown");					\
    throw CONCAT(type, Error)(msg);				\
  } while (false)						\

namespace snackis {
  struct Error: std::runtime_error {
    Error(const str &msg);
  };

  struct CoreError: Error {
    CoreError(const str &msg);
  };
}

#endif

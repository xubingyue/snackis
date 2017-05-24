#ifndef SNACKIS_ERROR_HPP
#define SNACKIS_ERROR_HPP

#include <stdexcept>

#include "snackis/core/macros.hpp"
#include "snackis/core/trace.hpp"

#define ERROR(type, msg) do {					\
    TRACE("Error thrown");					\
    throw CONCAT(type, Error)(msg);				\
  } while (false)						\

namespace snackis {
  struct Error: public std::runtime_error {
    Error(const std::string &msg);
  };

  struct CoreError: public Error {
    CoreError(const std::string &msg);
  };
}

#endif

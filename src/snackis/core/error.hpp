#ifndef SNACKIS_ERROR_HPP
#define SNACKIS_ERROR_HPP

#include <stdexcept>

#include "snackis/core/macros.hpp"

#define ERROR(type, msg)					\
  throw CONCAT(type, Error)(__FILE__, __LINE__, msg)		\

namespace snackis {
  struct Error: public std::runtime_error {
    Error(const char *file, int line, const std::string &msg);
  };
}

#endif

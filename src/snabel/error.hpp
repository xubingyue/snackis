#ifndef SNACKIS_SNABEL_ERROR_HPP
#define SNACKIS_SNABEL_ERROR_HPP

#include "snackis/core/error.hpp"

namespace snackis {
  struct SnabelError: Error {
    SnabelError(const str &msg);
  };
}

#endif

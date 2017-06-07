#ifndef SNACKIS_DB_ERROR_HPP
#define SNACKIS_DB_ERROR_HPP

#include "snackis/core/error.hpp"

namespace snackis {
  struct DbError: public Error {
    DbError(const str &msg);
  };
}

#endif

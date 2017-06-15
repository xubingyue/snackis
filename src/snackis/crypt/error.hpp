#ifndef SNACKIS_CRYPT_ERROR_HPP
#define SNACKIS_CRYPT_ERROR_HPP

#include "snackis/core/error.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  struct CryptError: Error {
    CryptError(const str &msg);
  };
}

#endif

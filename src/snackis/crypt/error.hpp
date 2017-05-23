#ifndef SNACKIS_CRYPT_ERROR_HPP
#define SNACKIS_CRYPT_ERROR_HPP

#include "snackis/core/error.hpp"

namespace snackis {
namespace crypt {
  struct CryptError: public Error {
    CryptError(const std::string &msg);
  };
}}

#endif

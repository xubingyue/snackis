#include "snackis/crypt/error.hpp"

namespace snackis {
  CryptError::CryptError(const str &msg):
    Error(str("CryptError: ") + msg) { }
}

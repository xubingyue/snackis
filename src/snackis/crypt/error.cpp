#include "snackis/crypt/error.hpp"

namespace snackis {
namespace crypt {

CryptError::CryptError(const str &msg):
    Error(str("CryptError: ") + msg) { }

}}

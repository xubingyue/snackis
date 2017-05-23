#include "snackis/crypt/error.hpp"

namespace snackis {
namespace crypt {

CryptError::CryptError(const std::string &msg):
    Error(std::string("CryptError: ") + msg) { }

}}

#include "snabel/error.hpp"

namespace snackis {

SnabelError::SnabelError(const str &msg):
    Error(str("SnabelError: ") + msg) { }
}

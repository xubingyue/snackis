#include "snackis/core/error.hpp"
#include "snackis/core/fmt.hpp"

namespace snackis {
  Error::Error(const std::string &msg): std::runtime_error(stack_trace() + msg) { }

  CoreError::CoreError(const std::string &msg):
    Error(std::string("CoreError: " + msg)) { }
}

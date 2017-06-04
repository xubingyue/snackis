#include "snackis/core/error.hpp"

namespace snackis {
  Error::Error(const str &msg): std::runtime_error(stack_trace() + msg) { }

  CoreError::CoreError(const str &msg):
    Error(str("CoreError: ") + msg) { }
}

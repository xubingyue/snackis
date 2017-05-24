#include "snackis/core/error.hpp"
#include "snackis/core/fmt.hpp"

namespace snackis {
  Error::Error(const str &msg): std::runtime_error(stack_trace() + msg) { }

  CoreError::CoreError(const str &msg):
    Error(str("CoreError: ") + msg) { }
}

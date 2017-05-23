#include "snackis/core/error.hpp"
#include "snackis/core/fmt.hpp"

namespace snackis {
  Error::Error(const char *file, int line, const std::string &msg):
    std::runtime_error((fmt("Error in file %1%, line %2%:\n%3%")
			% file % line % msg).str())
  { }
}

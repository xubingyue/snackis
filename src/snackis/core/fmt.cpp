#include "fmt.hpp"

namespace snackis {
  str fmt_arg(const char *arg) { return arg; }

  str fmt_arg(const str &arg) { return arg; }
}

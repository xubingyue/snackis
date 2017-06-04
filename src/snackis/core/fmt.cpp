#include "fmt.hpp"

namespace snackis {
  template <>
  str fmt_arg(const char * const &arg) { return arg; }

  template <>
  str fmt_arg(const str &arg) { return arg; }
}

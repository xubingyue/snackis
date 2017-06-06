#include <vector>
#include "snackis/core/uid.hpp"

namespace snackis {
  UId::UId(bool init) {
    if (init) { uuid_generate_random(val); }
    else { uuid_clear(val); }
  }

  bool operator==(const UId &x, const UId &y) {
    return uuid_compare(x.val, y.val) == 0;
  }

  bool operator<(const UId &x, const UId &y) {
    return uuid_compare(x.val, y.val) < 0;
  }

  template <>
  str fmt_arg(const UId &arg) {
    char cs[37];
    uuid_unparse_lower(arg.val, cs);
    return cs;
  }
}

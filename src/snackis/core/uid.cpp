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

  bool operator!=(const UId &x, const UId &y) {
    return uuid_compare(x.val, y.val) != 0;
  }

  bool operator<(const UId &x, const UId &y) {
    return uuid_compare(x.val, y.val) < 0;
  }

  str to_str(const UId &in) {
    char cs[37];
    uuid_unparse_lower(in.val, cs);
    return cs;
  }

  template <>
  str fmt_arg(const UId &arg) {
    return to_str(arg);
  }

  opt<UId> parse_uid(const str &in) {
    UId out;
    if (uuid_parse(in.c_str(), out.val) != 0) { return nullopt; }
    return out;
  }
}

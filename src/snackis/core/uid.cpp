#include <vector>
#include "snackis/core/uid.hpp"

namespace snackis {
  UId::UId(bool init) {
    if (init) { uuid_generate_random(val); }
  }

  bool operator<(const UId &x, const UId &y) {
    return uuid_compare(x.val, y.val) < 0;
  }
}

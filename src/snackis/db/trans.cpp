#include "snackis/db/trans.hpp"

namespace snackis {
  void log_change(Trans &trans, const Change *change) {
    trans.changes.push_back(change);
  }
}

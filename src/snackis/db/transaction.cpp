#include "snackis/db/transaction.hpp"

namespace snackis {
  void log_change(Transaction &trans, const Change *change) {
    trans.changes.push_back(change);
  }
}

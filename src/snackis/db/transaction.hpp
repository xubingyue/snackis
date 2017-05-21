#ifndef SNACKIS_TRANSACTION_HPP
#define SNACKIS_TRANSACTION_HPP

#include <vector>

namespace snackis {
  struct Change {
    virtual void rollback() = 0;
  };

  struct Transaction {
    std::vector<const Change *> changes;
  };

  void log_change(Transaction &trans, const Change *change);
}

#endif

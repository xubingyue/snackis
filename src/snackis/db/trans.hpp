#ifndef SNACKIS_TRANS_HPP
#define SNACKIS_TRANS_HPP

#include <vector>

namespace snackis {
  struct Change {
    virtual void rollback() = 0;
  };

  struct Trans {
    std::vector<const Change *> changes;
  };

  void log_change(Trans &trans, const Change *change);
}

#endif

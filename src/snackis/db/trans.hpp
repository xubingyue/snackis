#ifndef SNACKIS_TRANS_HPP
#define SNACKIS_TRANS_HPP

#include <vector>

namespace snackis {
  struct Change {
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
  };

  struct Ctx;
  
  struct Trans {
    Ctx &ctx;
    Trans *super;
    std::vector<const Change *> changes;
    Trans(Ctx &ctx);
    Trans(Ctx &ctx, Trans *super);
    ~Trans();
  };

  void log_change(Trans &trans, const Change *change);

  void commit(Trans &trans);
  void rollback(Trans &trans);
}

#endif

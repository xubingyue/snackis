#ifndef SNACKIS_DB_TRANS_HPP
#define SNACKIS_DB_TRANS_HPP

#include <mutex>
#include <vector>

namespace snackis {
namespace db {
  struct Change {
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
  };

  struct Ctx;
  
  struct Trans {
    Ctx &ctx;
    Trans *super;
    std::recursive_mutex mutex;
    std::vector<const Change *> changes;
    Trans(Ctx &ctx);
    ~Trans();
  };

  void log_change(Trans &trans, const Change *change);

  void commit(Trans &trans);
  void rollback(Trans &trans);
}}

#endif

#ifndef SNACKIS_DB_TRANS_HPP
#define SNACKIS_DB_TRANS_HPP

#include <vector>
#include "snackis/core/opt.hpp"

namespace snackis {
namespace db {
  struct Change {
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
  };

  struct Ctx;
  
  struct Trans {
    Ctx &ctx;
    opt<Trans &> super;
    std::vector<const Change *> changes;
    Trans(Ctx &ctx);
    ~Trans();
  };

  void log_change(Trans &trans, const Change *change);

  void commit(Trans &trans);
  void rollback(Trans &trans);
}}

#endif

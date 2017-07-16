#ifndef SNACKIS_DB_TRANS_HPP
#define SNACKIS_DB_TRANS_HPP

#include <vector>
#include "snackis/db/ctx.hpp"

namespace snackis {
namespace db {
  struct Change;
  
  struct Trans {
    Ctx &ctx;
    Trans *super;
    Ctx::Lock lock; 
    std::vector<Change *> changes;
    Trans(Ctx &ctx);
    ~Trans();
  };

  void log_change(Trans &trans, Change *change);
  void commit(Trans &trans, const opt<str> &lbl);
  void rollback(Trans &trans);
}}

#endif

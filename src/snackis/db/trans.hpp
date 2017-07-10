#ifndef SNACKIS_DB_TRANS_HPP
#define SNACKIS_DB_TRANS_HPP

#include <vector>

namespace snackis {
namespace db {
  struct Change;
  struct Ctx;
  
  struct Trans {
    Ctx &ctx;
    Trans *super;
    std::unique_lock<std::recursive_mutex> lock; 
    std::vector<Change *> changes;
    Trans(Ctx &ctx);
    ~Trans();
  };

  void log_change(Trans &trans, Change *change);
  void commit(Trans &trans, const opt<str> &lbl);
  void rollback(Trans &trans);
}}

#endif

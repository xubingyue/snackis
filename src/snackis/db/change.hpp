#ifndef SNACKIS_DB_CHANGE_HPP
#define SNACKIS_DB_CHANGE_HPP

#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
namespace db {
  struct Ctx;
  
  struct Change {
    virtual ~Change();
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
    virtual void undo() const = 0;
  };

  struct ChangeSet {
    Ctx &ctx;
    str label;
    Time committed_at;
    std::vector<Change *> changes;

    ChangeSet(Ctx &ctx, const str &lbl, std::vector<Change *> &chs);
    ~ChangeSet();
  };

  void undo(ChangeSet &cs);
}}

#endif

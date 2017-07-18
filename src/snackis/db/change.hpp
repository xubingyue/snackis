#ifndef SNACKIS_DB_CHANGE_HPP
#define SNACKIS_DB_CHANGE_HPP

#include <memory>
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
namespace db {
  struct Ctx;
  
  struct Change {
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
    virtual void undo() const = 0;
  };

  using Changes = std::vector<std::shared_ptr<Change>>;

  struct ChangeSet {
    Ctx &ctx;
    str label;
    Time committed_at;
    Changes changes;

    ChangeSet(Ctx &ctx, const str &lbl, Changes &chs);
  };

  void undo(ChangeSet &cs);
}}

#endif

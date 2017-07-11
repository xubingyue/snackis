#include "snackis/core/error.hpp"
#include "snackis/db/change.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  Change::~Change()
  { }

  ChangeSet::ChangeSet(Ctx &ctx, const str &lbl, std::vector<Change *> &chs):
    ctx(ctx), label(lbl), committed_at(now())
  {
    changes.swap(chs);
  }
  
  ChangeSet::~ChangeSet() {
    for (auto c: changes) { delete c; }
  }

  void undo(ChangeSet &cs) {
    db::Trans trans(cs.ctx);
    TRY(try_undo);
    
    for (auto c: cs.changes) { c->undo(); }

    if (try_undo.errors.empty()) {
      db::commit(trans, nullopt);
    }
  }
}}

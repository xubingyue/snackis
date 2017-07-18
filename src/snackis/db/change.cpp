#include "snackis/core/error.hpp"
#include "snackis/db/change.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  ChangeSet::ChangeSet(Ctx &ctx, const str &lbl, Changes &chs):
    ctx(ctx), label(lbl), committed_at(now())
  {
    changes.swap(chs);
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

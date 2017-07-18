#include "snackis/db/change.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  Trans::Trans(Ctx &ctx):
    ctx(ctx), lock(ctx.mutex)
  {
    super = ctx.trans;
    ctx.trans = this;
  }

  Trans::~Trans() {
    if (!changes.empty()) { rollback(*this); }
    ctx.trans = super;
  }
  
  void log_change(Trans &trans, Change *change) {
    trans.changes.push_back(std::shared_ptr<Change>(change));
  }

  static void clear(Trans &trans) {
    trans.changes.clear();
  }
  
  void commit(Trans &trans, const opt<str> &lbl) {
    if (trans.changes.empty()) { return; }
    for (auto &c: trans.changes) { c->commit(); }
    flush(trans.ctx);
    
    if (lbl) {
      trans.ctx.undo_stack.emplace_back(trans.ctx, *lbl, trans.changes);
    } else {
      clear(trans);
    }
  }
  
  void rollback(Trans &trans) {
    for (auto &c: trans.changes) { c->rollback(); }
    clear(trans);
  }
}}

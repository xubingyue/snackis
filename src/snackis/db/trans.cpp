#include "snackis/db/ctx.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  Trans::Trans(Ctx &ctx): ctx(ctx) {
    ctx.mutex.lock();
    super = ctx.trans;
    ctx.trans = this;
  }

  Trans::~Trans() {
    if (!changes.empty()) { rollback(*this); }
    ctx.trans = super;
    ctx.mutex.unlock();
  }
  
  void log_change(Trans &trans, const Change *change) {
    trans.changes.push_back(change);
  }

  void commit(Trans &trans) {
    for (const Change *c: trans.changes) { c->commit(); }
    flush(trans.ctx);
    trans.changes.clear();
  }
  
  void rollback(Trans &trans) {
    for (const Change *c: trans.changes) { c->rollback(); }
    trans.changes.clear();
  }
}}

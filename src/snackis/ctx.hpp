#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include <condition_variable>
#include <mutex>
#include <thread>
#include "snackis/db.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  struct Ctx: db::Ctx {
    using LoopLock = std::unique_lock<std::mutex>;
    Db db;
    Settings settings;
    Peer whoami;
    std::mutex loop_mutex;
    std::condition_variable fetch_cond, send_cond;
    opt<std::thread> fetcher, sender;
    bool closing;
    Ctx(db::Proc &p, size_t max_buf);
    virtual ~Ctx();
  };

  void open(Ctx &ctx);
  void log(const Ctx &ctx, const str &msg);
  Peer &whoami(Ctx &ctx);
  int64_t rewrite_db(Ctx &ctx);

  template <typename RecT>
  void rewrite_table(db::Table<RecT> &tbl) {
    std::vector<RecT> del_recs;
    
    for (auto i(tbl.recs.begin()); i != tbl.recs.end(); i++) {
      RecT rec(dynamic_cast<Ctx &>(tbl.ctx), *i);
      if (rec.owner_id == null_uid) { del_recs.push_back(rec); }
    }

    for (auto &rec: del_recs) { db::erase(tbl, rec); }
  }
}

#endif

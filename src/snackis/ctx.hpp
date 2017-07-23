#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include "snackis/db.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  struct Ctx: db::Ctx {
    Db db;
    Settings settings;
    Ctx(db::Proc &p, size_t max_buf);
  };

  void open(Ctx &ctx);
  void log(const Ctx &ctx, const str &msg);
  UId whoamid(Ctx &ctx);
  Peer whoami(Ctx &ctx);
  int64_t rewrite_db(Ctx &ctx);
}

#endif

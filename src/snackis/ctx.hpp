#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include "snackis/db.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/mutex.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  struct Ctx {
    using Log = std::function<void (const str &)>;

    Db db;
    Settings settings;
    opt<Log> log;
    Mutex log_mutex;
    
    Ctx(db::Ctx &db_ctx);
  };

  void log(const Ctx &ctx, const str &msg);
}

#endif

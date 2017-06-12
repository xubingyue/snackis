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
  struct Ctx: public db::Ctx {
    Db db;
    Settings settings;
    Peer whoami;
    std::mutex fetch_mutex, send_mutex;
    std::condition_variable fetch_cond, send_cond;
    std::thread fetcher, sender;
    bool is_closing;
    Ctx(const Path &path);
    virtual ~Ctx();
  };

  void open(Ctx &ctx);
  void log(const Ctx &ctx, const str &msg);
  Peer &whoami(Ctx &ctx);
}

#endif

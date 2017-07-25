#ifndef SNACKIS_DB_CTX_HPP
#define SNACKIS_DB_CTX_HPP

#include <mutex>
#include <list>
#include <set>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/change.hpp"
#include "snackis/db/msg.hpp"
#include "snackis/db/proc.hpp"

namespace snackis {
namespace db {
  struct BasicTable;

  template <typename RecT, typename...KeyT>
  struct Table;

  struct Trans;
  
  struct Ctx {
    Proc &proc;
    Chan<Msg> inbox;
    opt<crypt::Secret> secret;
    std::map<str, BasicTable *> tables;
    Trans *trans;
    std::list<ChangeSet> undo_stack;
    
    Ctx(Proc &p, size_t max_buf);
    virtual ~Ctx();
  };

  Path get_path(const Ctx &ctx, const str &fname);
  Trans &get_trans(Ctx &ctx);
  bool pass_exists(const Ctx &ctx);
  void init_pass(Ctx &ctx, const str &pass);
  bool login(Ctx &ctx, const str &pass);
  void open(Ctx &ctx);
  void slurp(Ctx &ctx);
  int64_t rewrite(Ctx &ctx);
  int64_t refresh(Ctx &ctx);

  template <typename...Args>
  void log(const Ctx &ctx, const str &spec, const Args&...args) {
    log(ctx.proc, spec, args...);
  }

  template <typename RecT, typename...KeyT>
  Table<RecT, KeyT...> &get_table(const Ctx &ctx, const str &name) {
    auto fnd(ctx.tables.find(name));
    CHECK(fnd != ctx.tables.end(), _);
    return *dynamic_cast<Table<RecT, KeyT...> *>(fnd->second);
  }
}}

#endif

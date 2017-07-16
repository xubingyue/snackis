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

namespace snackis {
namespace db {
  struct BasicTable;
  struct Trans;
  
  struct Ctx {
    using Log = func<void (const str &)>;
    using Lock = std::unique_lock<std::recursive_mutex>;
    
    Path path;
    opt<crypt::Secret> secret;
    opt<Log> log;
    std::set<BasicTable *> tables;
    Trans *trans;
    std::set<std::ostream *> dirty_files;
    std::list<ChangeSet> undo_stack;
    std::recursive_mutex mutex;

    Ctx(const Path &path);
    virtual ~Ctx();
  };

  Path get_path(const Ctx &ctx, const str &fname);
  Trans &get_trans(Ctx &ctx);
  void init_db_ver(Ctx &ctx);
  bool pass_exists(const Ctx &ctx);
  void init_pass(Ctx &ctx, const str &pass);
  bool login(Ctx &ctx, const str &pass);
  void open(Ctx &ctx);
  void dirty_file(Ctx &ctx, std::ostream &file);
  void flush(Ctx &ctx);
  void slurp(Ctx &ctx);
  int64_t defrag(Ctx &ctx);

  template <typename...Args>
  void log(const Ctx &ctx, const str &spec, const Args &...args) {
    if (ctx.log) { (*ctx.log)(fmt(spec, std::forward<const Args &>(args)...)); }
  }
}}

#endif

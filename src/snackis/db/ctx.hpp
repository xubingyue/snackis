#ifndef SNACKIS_DB_CTX_HPP
#define SNACKIS_DB_CTX_HPP

#include <set>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/mutex.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  struct BasicTable;
  
  struct Ctx {
    using Log = func<void (const str &)>;

    const Path path;
    opt<crypt::Secret> secret;
    opt<Log> log;
    Mutex log_mutex;
    std::set<BasicTable *> tables;
    opt<Trans &> trans;
    std::set<std::ostream *> dirty_files;
    
    Ctx(const Path &path);
    virtual ~Ctx();
  };

  Path get_path(const Ctx &ctx, const str &fname);
  void log(const Ctx &ctx, const str &msg);
  bool pass_exists(const Ctx &ctx);
  void init_pass(Ctx &ctx, const str &pass);
  bool login(Ctx &ctx, const str &pass);
  void open(Ctx &ctx);
  void dirty_file(Ctx &ctx, std::ostream &file);
  void flush(Ctx &ctx);
  void slurp(Ctx &ctx);
}}

#endif

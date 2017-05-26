#ifndef SNACKIS_DB_CTX_HPP
#define SNACKIS_DB_CTX_HPP

#include "snackis/core/opt.hpp"
#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  struct Ctx {
    const Path path;
    opt<crypt::Secret> secret;
    Trans root;
    Trans *trans;
    
    Ctx(const Path &path);
    virtual ~Ctx();
  };

  bool pass_exists(const Ctx &ctx);
  void init_pass(Ctx &ctx, const str &pass);
  bool login(Ctx &ctx, const str &pass);
  Path get_path(const Ctx &ctx, const str &fname);
  void commit(Ctx &ctx);
}}

#endif

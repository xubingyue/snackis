#ifndef SNACKIS_DB_CTX_HPP
#define SNACKIS_DB_CTX_HPP

#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/secret.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
namespace db {
  struct Ctx {
    const Path path;
    const crypt::Secret *secret;
    Trans root;
    Trans *trans;
    
    Ctx(const Path &path);
    virtual ~Ctx();
  };

  str get_path(const Ctx &ctx, const str &fname);
  void commit(Ctx &ctx);
}}

#endif

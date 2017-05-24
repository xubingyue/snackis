#ifndef SNACKIS_DB_CTX_HPP
#define SNACKIS_DB_CTX_HPP

#include "snackis/core/path.hpp"
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

  std::string get_path(const Ctx &ctx, const std::string &fname);
  void commit(Ctx &ctx);
}}

#endif

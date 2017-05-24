#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include "snackis/db.hpp"
#include "snackis/settings.hpp"

namespace snackis {
  struct Ctx {
    Db db;
    Settings settings;

    Ctx(db::Ctx &db_ctx);
  };
}

#endif

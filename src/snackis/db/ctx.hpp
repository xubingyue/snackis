#ifndef SNACKIS_CTX_HPP
#define SNACKIS_CTX_HPP

#include <snackis/core/path.hpp>
#include <snackis/db/trans.hpp>

namespace snackis {
  struct Ctx {
    const Path path;
    Trans root;
    Trans *trans;
    
    Ctx(const Path &path);
  };

  std::string get_path(const Ctx &ctx, const std::string &fname);
  void commit(Ctx &ctx);
}

#endif

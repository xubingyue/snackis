#include "ctx.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path): path(path), root(*this), trans(&root) {
    create(path);
  }

  std::string get_path(const Ctx &ctx, const std::string &fname) {
    return (ctx.path / fname).string(); 
  }
}

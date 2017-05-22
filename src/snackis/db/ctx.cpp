#include "ctx.hpp"

namespace snackis {
  Ctx::Ctx(const Path &path): path(path), trans(new Trans()) {
    create(path);
  }

  std::string get_path(const Ctx &ctx, const std::string &fname) {
    return (ctx.path / fname).string(); 
  }
}

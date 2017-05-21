#include "context.hpp"

namespace snackis {
  Context::Context(const Path &path):
    path(path), transaction(new Transaction()) { }

  std::string get_path(const Context &ctx, const std::string &fname) {
    return (ctx.path / fname).string(); 
  }
}

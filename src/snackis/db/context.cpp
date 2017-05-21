#include "context.hpp"

namespace snackis {
  Context::Context(const Path &path): path(path) {
  }

  string get_path(const Context &ctx, const string &fname) {
    return (ctx.path / fname).string(); 
  }
}

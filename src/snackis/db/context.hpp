#ifndef SNACKIS_CONTEXT_HPP
#define SNACKIS_CONTEXT_HPP

#include <snackis/core/path.hpp>

namespace snackis {
  struct Context {
    const Path path;
    
    Context(const Path &path);
  };

  std::string get_path(const Context &ctx, const std::string &fname);
}

#endif

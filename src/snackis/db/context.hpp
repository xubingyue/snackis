#ifndef SNACKIS_CONTEXT_HPP
#define SNACKIS_CONTEXT_HPP

#include <snackis/core/path.hpp>

namespace snackis {
  using namespace std;
  
  struct Context {
    const Path path;
    
    Context(const Path &path);
  };

  string get_path(const Context &ctx, const string &fname);
}

#endif

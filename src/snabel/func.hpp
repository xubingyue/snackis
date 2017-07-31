#ifndef SNABEL_FUNC_HPP
#define SNABEL_FUNC_HPP

#include <vector>
#include "snackis/core/func.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Ctx;
  struct Type;
  
  struct Func {
    using Args = std::vector<Type *>;
    using Imp = func<void (Ctx &, Func &)>;

    Args args;
    Imp imp;

    Func(const Args &args, Imp imp):
      args(args), imp(imp)
    { }

    void operator ()(Ctx &ctx) {
      imp(ctx, *this);
    }
  };
}

#endif

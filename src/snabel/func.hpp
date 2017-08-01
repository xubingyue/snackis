#ifndef SNABEL_FUNC_HPP
#define SNABEL_FUNC_HPP

#include <list>
#include <vector>

#include "snackis/core/func.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Ctx;
  struct Func;
  struct Type;
  
  struct FuncImp {
    using Args = std::vector<Type *>;
    using Imp = func<void (Ctx &, FuncImp &)>;

    Func &func;
    Args args;
    Imp imp;

    FuncImp(Func &fn, const Args &args, Imp imp);    
    void operator ()(Ctx &ctx);
  };

  struct Func {
    std::list<FuncImp> imps;
  };

  FuncImp &add_imp(Func &fn, const FuncImp::Args &args, FuncImp::Imp imp);
}

#endif

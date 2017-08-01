#ifndef SNABEL_FUNC_HPP
#define SNABEL_FUNC_HPP

#include <list>
#include <vector>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"

namespace snabel {
  using namespace snackis;

  struct BasicType;
  struct Box;
  struct Ctx;
  struct Func;
  
  struct FuncImp {
    using Args = std::vector<BasicType *>;
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
  opt<FuncImp> match(const Func &fn, const std::vector<Box> &args);
  void call(Func &fn, Ctx &ctx);
}

#endif

#ifndef SNABEL_FUNC_HPP
#define SNABEL_FUNC_HPP

#include <list>
#include <deque>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/uid.hpp"

namespace snabel {
  using namespace snackis;

  struct BasicType;
  struct Box;
  struct Ctx;
  struct Func;
  
  struct FuncImp {
    using Args = std::deque<BasicType *>;
    using Imp = func<void (Ctx &, FuncImp &)>;

    Func &func;
    Args args;
    Imp imp;

    FuncImp(Func &fn, const Args &args, Imp imp);    
    void operator ()(Ctx &ctx);
  };

  struct Func {
    str name;
    std::list<FuncImp> imps;
    Func(const str &nam);
  };

  using ArgSeq = std::deque<Box>;
  
  ArgSeq get_args(const FuncImp imp, Ctx &ctx);
  FuncImp &add_imp(Func &fn, const FuncImp::Args &args, FuncImp::Imp imp);
  opt<FuncImp> match(const Func &fn, const ArgSeq &args);
  void call(Func &fn, Ctx &ctx);
}

#endif

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
  struct Coro;
  struct Scope;
  struct Func;
  
  using ArgSeq = std::deque<Box>;

  struct FuncImp {
    using Args = std::deque<BasicType *>;
    using Imp = func<void (Scope &, FuncImp &, const ArgSeq &)>;

    Func &func;
    Args args;
    Type &res_type;
    Imp imp;

    FuncImp(Func &fn, const Args &args, Type &rt, Imp imp);    
    void operator ()(Coro &cor);
  };

  struct Func {
    str name;
    std::list<FuncImp> imps;
    Func(const str &nam);
  };
  
  ArgSeq pop_args(const FuncImp &imp, Coro &cor);
  FuncImp &add_imp(Func &fn,
		   const FuncImp::Args &args,
		   Type &rt,
		   FuncImp::Imp imp);
  FuncImp *match(Func &fn, const Coro &cor);
}

#endif

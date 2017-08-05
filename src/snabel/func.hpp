#ifndef SNABEL_FUNC_HPP
#define SNABEL_FUNC_HPP

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
  
  using ArgTypes = std::deque<BasicType *>;
  using Args = std::deque<Box>;

  struct FuncImp {
    using Imp = func<void (Scope &, FuncImp &, const Args &)>;

    Func &func;
    ArgTypes args;
    Type &res_type;
    Imp imp;
    bool pure;
    
    FuncImp(Func &fn, const ArgTypes &args, Type &rt, Imp imp, bool pur=true);
    void operator ()(Coro &cor, const Args &args);
    void operator ()(Coro &cor);
  };

  struct Func {
    str name;
    std::deque<FuncImp> imps;
    Func(const str &nam);
  };
  
  Args pop_args(const FuncImp &imp, Coro &cor);
  FuncImp &add_imp(Func &fn,
		   const ArgTypes &args,
		   Type &rt,
		   FuncImp::Imp imp);
  FuncImp *match(Func &fn, const Coro &cor);
}

#endif

#include "snabel/func.hpp"

namespace snabel {
  FuncImp::FuncImp(Func &fn, const Args &args, Imp imp):
    func(fn), args(args), imp(imp)
  { }
  
  void FuncImp::operator ()(Ctx &ctx) {
    imp(ctx, *this);
  }

  FuncImp &add_imp(Func &fn, const FuncImp::Args &args, FuncImp::Imp imp) {
    return fn.imps.emplace_back(fn, args, imp);
  }
}

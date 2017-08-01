#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/ctx.hpp"
#include "snabel/error.hpp"
#include "snabel/func.hpp"
#include "snabel/type.hpp"

namespace snabel {
  FuncImp::FuncImp(Func &fn, const Args &args, Imp imp):
    func(fn), args(args), imp(imp)
  { }
  
  void FuncImp::operator ()(Ctx &ctx) {
    imp(ctx, *this);
  }

  FuncImp &add_imp(Func &fn, const FuncImp::Args &args, FuncImp::Imp imp) {
    return fn.imps.emplace_front(fn, args, imp);
  }

  static bool match(const FuncImp imp, const std::vector<Box> &args) {
    auto i = args.rbegin();
    auto j = imp.args.rbegin();
    
    while (i != args.rend() && j != imp.args.rend()) {
      auto seq(dynamic_cast<TypeSeq *>(*j));

      if (!isa(*i, **j)) {
	j++;
      } else {
	if (!seq) { return false; }
	j++;
      }

      i++;
    }

    return true;
  }
  
  opt<FuncImp> match(const Func &fn, const std::vector<Box> &args) {
    for (auto &imp: fn.imps) {
      if (match(imp, args)) { return imp; }
    }

    return nullopt;
  }

  void call(Func &fn, Ctx &ctx) {
    auto imp(match(fn, ctx.coro.stack));
    
    if (!imp) {
      ERROR(Snabel, fmt("Function not applicable:\n%0", 
			ctx.coro.stack));
      return;
    }
    
    Ctx tmp(ctx);
    (*imp)(tmp);
  }
}

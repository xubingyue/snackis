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

  Func::Func(const str &nam):
    name(nam)
  { }

  ArgSeq get_args(const FuncImp imp, Ctx &ctx) {
    auto i = imp.args.rbegin();
    ArgSeq out;
    
    while (i != imp.args.rend() && !ctx.coro.stack->empty()) {
      auto &typ(*i);
      auto seq(dynamic_cast<Seq *>(typ));
      auto &val(ctx.coro.stack->back());

      if (!isa(val, *typ) && (!seq || !isa(val, seq->elem_type))) {
	break;
      }
      
      out.push_back(val);
      ctx.coro.stack->pop_back();
      if (!seq) { i++; }
    }

    return ArgSeq(out.rbegin(), out.rend());
  }

  FuncImp &add_imp(Func &fn, const FuncImp::Args &args, FuncImp::Imp imp) {
    return fn.imps.emplace_front(fn, args, imp);
  }

  static bool match(const FuncImp imp, const ArgSeq &args) {
    auto i = args.rbegin();
    auto j = imp.args.rbegin();
    
    while (i != args.rend() && j != imp.args.rend()) {
      auto seq(dynamic_cast<Seq *>(*j));

      if (isa(*i, **j) || (seq && isa(*i, seq->elem_type))) {
	if (!seq) { j++; }
      } else {
	if (!seq) { return false; }
	j++;
      }

      i++;
    }

    return true;
  }

  opt<FuncImp> match(const Func &fn, const ArgSeq &args) {
    for (auto &imp: fn.imps) {
      if (match(imp, args)) { return imp; }
    }

    return nullopt;
  }

  void call(Func &fn, Ctx &ctx) {
    auto imp(match(fn, *ctx.coro.stack));
    
    if (!imp) {
      ERROR(Snabel, fmt("Function not applicable:\n%0", 
			*ctx.coro.stack));
      return;
    }
    
    Ctx tmp(ctx);
    (*imp)(tmp);
  }
}

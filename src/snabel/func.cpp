#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/scope.hpp"
#include "snabel/error.hpp"
#include "snabel/func.hpp"
#include "snabel/type.hpp"

namespace snabel {
  FuncImp::FuncImp(Func &fn, const Args &args, Type &rt, Imp imp):
    func(fn), args(args), res_type(rt), imp(imp)
  { }
  
  void FuncImp::operator ()(Coro &cor) {
    auto args(pop_args(*this, cor));
    Scope &tmp(begin_scope(cor));
    imp(tmp, *this, args);
    end_scope(cor);
 }

  Func::Func(const str &nam):
    name(nam)
  { }

  ArgSeq pop_args(const FuncImp &imp, Coro &cor) {
    auto i = imp.args.rbegin();
    auto &s(curr_stack(cor));
    ArgSeq out;
    
    while (i != imp.args.rend() && !s.empty()) {
      auto &typ(*i);
      auto seq(dynamic_cast<Seq *>(typ));
      auto &val(s.back());

      if (!isa(val, *typ) && (!seq || !isa(val, seq->elem_type))) {
	break;
      }
      
      out.push_back(val);
      s.pop_back();
      if (!seq) { i++; }
    }

    return ArgSeq(out.rbegin(), out.rend());
  }

  FuncImp &add_imp(Func &fn,
		   const FuncImp::Args &args,
		   Type &rt,
		   FuncImp::Imp imp) {
    return fn.imps.emplace_front(fn, args, rt, imp);
  }

  static bool match(const FuncImp &imp, const Coro &cor) {
    auto &s(curr_stack(cor));
    auto i = s.rbegin();
    auto j = imp.args.rbegin();
    
    while (i != s.rend() && j != imp.args.rend()) {
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

  FuncImp *match(Func &fn, const Coro &cor) {
    for (auto &imp: fn.imps) {
      if (match(imp, cor)) { return &imp; }
    }

    return nullptr;
  }
}

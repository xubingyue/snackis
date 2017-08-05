#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/scope.hpp"
#include "snabel/error.hpp"
#include "snabel/func.hpp"
#include "snabel/type.hpp"

namespace snabel {
  FuncImp::FuncImp(Func &fn, const ArgTypes &args, Type &rt, Imp imp, bool pur):
    func(fn), args(args), res_type(rt), imp(imp), pure(pur)
  { }
  
  void FuncImp::operator ()(Coro &cor, const Args &args) {
    Scope &tmp(begin_scope(cor, false));
    imp(tmp, *this, args);
    end_scope(cor);
  }

  void FuncImp::operator ()(Coro &cor) {
    auto args(pop_args(*this, cor));
    (*this)(cor, args);
  }

  Func::Func(const str &nam):
    name(nam)
  { }

  Args pop_args(const FuncImp &imp, Coro &cor) {
    auto i = imp.args.rbegin();
    auto &s(curr_stack(cor));
    Args out;
    
    while (i != imp.args.rend() && !s.empty()) {
      auto &typ(*i);
      auto &val(s.back());
      if (!isa(val, *typ)) { break; }
      out.push_back(val);
      s.pop_back();
      i++;
    }

    return Args(out.rbegin(), out.rend());
  }

  FuncImp &add_imp(Func &fn,
		   const ArgTypes &args,
		   Type &rt,
		   FuncImp::Imp imp) {
    return fn.imps.emplace_front(fn, args, rt, imp);
  }

  static bool match(const FuncImp &imp, const Coro &cor) {
    if (imp.args.empty()) { return true; }
    auto &s(curr_stack(cor));
    if (s.size() < imp.args.size()) { return false; }
    auto i(s.rbegin());
    auto j(imp.args.rbegin());
    
    while (i != s.rend() && j != imp.args.rend()) {
      if (!isa(*i, **j)) { return false; }
      i++;
      j++;
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

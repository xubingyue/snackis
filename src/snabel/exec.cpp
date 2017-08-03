#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/type.hpp"

namespace snabel {
  static void add_i64(Scope &scp, FuncImp &fn, const Args &args) {
    Exec &exe(scp.coro.exec);
    int64_t res(0);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(scp.coro, exe.i64_type, res);
  }

  static void sub_i64(Scope &scp, FuncImp &fn, const Args &args) {
    Exec &exe(scp.coro.exec);
    int64_t res(get<int64_t>(args[0]));

    if (args.size() == 1) { res = -res; }
    else {
      for (auto i=std::next(args.begin()); i != args.end(); i++) {
	CHECK(&i->type == &exe.i64_type, _);
	res -= get<int64_t>(*i);
      }
    }
    
    push(scp.coro, exe.i64_type, res);
  }

  static void mul_i64(Scope &scp, FuncImp &fn, const Args &args) {
    Exec &exe(scp.coro.exec);
    int64_t res(1);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res *= get<int64_t>(a);
    }
    
    push(scp.coro, exe.i64_type, res);
  }

  static void mod_i64(Scope &scp, FuncImp &fn, const Args &args) {
    Exec &exe(scp.coro.exec);
    int64_t res(get<int64_t>(args[0]));
    for (auto i=std::next(args.begin()); i != args.end(); i++) {
      CHECK(&i->type == &exe.i64_type, _);
      res %= get<int64_t>(*i);
    }
    
    push(scp.coro, exe.i64_type, res);
  }
  
  Exec::Exec():
    main(fibers.emplace(std::piecewise_construct,
			std::forward_as_tuple(null_uid),
			std::forward_as_tuple(*this, null_uid)).first->second),
    meta_type((add_type(main, "Type"))),
    op_type((add_type(main, "Op"))),
    op_seq_type((add_type(main, "OpSeq"))),
    func_type((add_type(main, "Func"))),
    i64_type((add_type(main, "I64"))),
    str_type((add_type(main, "Str"))),
    void_type((add_type(main, "Void")))
  {
    meta_type.fmt = [](auto &v) { return get<Type *>(v)->name; };
    op_type.fmt = [](auto &v) { return fmt_arg(get<Op>(v).code); };
    op_seq_type.fmt = [](auto &v) { return fmt_arg(get<OpSeq>(v).size()); };
    func_type.fmt = [](auto &v) { return fmt_arg(size_t(get<Func *>(v))); };
    i64_type.fmt = [](auto &v) { return fmt_arg(get<int64_t>(v)); };
    str_type.fmt = [](auto &v) { return get<str>(v); };

    Func &add(add_func(main, "+"));
    add_imp(add, {&i64_type.seq}, i64_type, add_i64);

    Func &sub(add_func(main, "-"));
    add_imp(sub, {&i64_type.seq}, i64_type, sub_i64);
    
    Func &mul(add_func(main, "*"));
    add_imp(mul, {&i64_type.seq}, i64_type, mul_i64);

    Func &mod(add_func(main, "%"));
    add_imp(mod, {&i64_type.seq}, i64_type, mod_i64);
  }
}

#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/type.hpp"

namespace snabel {
  static void add_i64(Scope &scp, FuncImp &fn) {
    auto args(get_args(fn, scp));
    Exec &exe(scp.coro.exec);
    int64_t res(0);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(scp.coro, exe.i64_type, res);
  }

  static void mul_i64(Scope &scp, FuncImp &fn) {
    auto args(get_args(fn, scp));
    Exec &exe(scp.coro.exec);
    int64_t res(1);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res *= get<int64_t>(a);
    }
    
    push(scp.coro, exe.i64_type, res);
  }
  
  Exec::Exec():
    main(fibers.emplace(std::piecewise_construct,
			std::forward_as_tuple(null_uid),
			std::forward_as_tuple(*this, null_uid)).first->second),
    scope(get_scope(main)),
    meta_type((add_type(scope, "Type"))),
    op_type((add_type(scope, "Op"))),
    op_seq_type((add_type(scope, "OpSeq"))),
    func_type((add_type(scope, "Func"))),
    i64_type((add_type(scope, "I64"))),
    str_type((add_type(scope, "Str")))
  {
    meta_type.fmt = [](auto &v) { return get<Type *>(v)->name; };
    op_type.fmt = [](auto &v) { return fmt_arg(get<Op>(v).code); };
    op_seq_type.fmt = [](auto &v) { return fmt_arg(get<OpSeq>(v).size()); };
    func_type.fmt = [](auto &v) { return fmt_arg(size_t(get<Func *>(v))); };
    i64_type.fmt = [](auto &v) { return fmt_arg(get<int64_t>(v)); };
    str_type.fmt = [](auto &v) { return get<str>(v); };

    Func &add(add_func(scope, "+"));
    add_imp(add, {&i64_type.seq}, add_i64);

    Func &mul(add_func(scope, "*"));
    add_imp(mul, {&i64_type.seq}, mul_i64);
  }
}

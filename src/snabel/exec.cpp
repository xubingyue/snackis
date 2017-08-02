#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/type.hpp"

namespace snabel {
  static void add_i64(Ctx &ctx, FuncImp &fn) {
    auto args(get_args(fn, ctx));
    Exec &exe(ctx.coro.exec);
    int64_t res(0);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(ctx.coro, exe.i64_type, res);
  }

  static void mul_i64(Ctx &ctx, FuncImp &fn) {
    auto args(get_args(fn, ctx));
    Exec &exe(ctx.coro.exec);
    int64_t res(1);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res *= get<int64_t>(a);
    }
    
    push(ctx.coro, exe.i64_type, res);
  }
  
  Exec::Exec():
    main(fibers.emplace(std::piecewise_construct,
			std::forward_as_tuple(null_uid),
			std::forward_as_tuple(*this, null_uid)).first->second),
    ctx(get_ctx(main)),
    meta_type((add_type(ctx, "Type"))),
    op_type((add_type(ctx, "Op"))),
    op_seq_type((add_type(ctx, "OpSeq"))),
    func_type((add_type(ctx, "Func"))),
    i64_type((add_type(ctx, "I64"))),
    str_type((add_type(ctx, "Str")))
  {
    meta_type.fmt = [](auto &v) { return get<Type *>(v)->name; };
    op_type.fmt = [](auto &v) { return fmt_arg(get<Op>(v).code); };
    op_seq_type.fmt = [](auto &v) { return fmt_arg(get<OpSeq>(v).size()); };
    func_type.fmt = [](auto &v) { return fmt_arg(size_t(get<Func *>(v))); };
    i64_type.fmt = [](auto &v) { return fmt_arg(get<int64_t>(v)); };
    str_type.fmt = [](auto &v) { return get<str>(v); };

    Func &add(add_func(ctx, "+"));
    add_imp(add, {&i64_type.seq}, add_i64);

    Func &mul(add_func(ctx, "*"));
    add_imp(mul, {&i64_type.seq}, mul_i64);
  }

  void compile(Exec &exe, const str &in) {
    Compiler cpr(exe.ctx);
    compile(cpr, in);
    std::copy(cpr.ops.begin(), cpr.ops.end(),
	      std::back_inserter(exe.main.ops));
  }

  void run(Exec &exe) {
    Coro &cor(exe.main);

    while (cor.pc < cor.ops.size()) {
      run(cor.ops[cor.pc], get_ctx(cor));
      cor.pc++;
    }
  }

  void reset(Exec &exe) {
    Coro &cor(exe.main);
    cor.ops.clear();
    cor.pc = 0;
    cor.stack->clear();
  }
}

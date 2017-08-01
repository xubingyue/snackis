#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/exec.hpp"
#include "snabel/op.hpp"
#include "snabel/type.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  static void add(Ctx &ctx, FuncImp &fn) {
    Exec &exe(ctx.coro.exec);
    int64_t res(0);

    for (auto &a: ctx.coro.stack) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(ctx.coro, exe.i64_type, res);
  }
  
  void func_tests() {
    TRY(try_test);
    
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));    
    Func &f(add_func(ctx, "+"));
    add_imp(f, {&exe.i64_type.seq}, add);
    
    run(ctx,
	{Push(exe.i64_type, int64_t(35)),
	    Push(exe.i64_type, int64_t(7)),
	    Call(f),
	    Let("foo")});
    
    CHECK(get<int64_t>(get_env(ctx, "foo")) == 42, _);
  }

  void compile_tests() {
    TRY(try_test);    
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));
    Func &f(add_func(ctx, "+"));
    add_imp(f, {&exe.i64_type.seq}, add);
    OpSeq ops(compile(ctx, "let foo 35\nlet bar foo 7 +"));
    run(ctx, ops);
    CHECK(get<int64_t>(get_env(ctx, "foo")) == 35, _);
    CHECK(get<int64_t>(get_env(ctx, "bar")) == 42, _);
  }

  void all_tests() {
    func_tests();
    compile_tests();
  }
}

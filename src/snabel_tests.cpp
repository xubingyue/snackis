#include <iostream>
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
    Func f;
    auto &fi(add_imp(f, {}, add)); 
    
    run(ctx,
	{Push(exe.i64_type, int64_t(35)),
	    Push(exe.i64_type, int64_t(7)),
	    Call(fi),
	    Bind("foo")});
    
    CHECK(get<int64_t>(get_env(ctx, "foo")) == 42, _);
    CHECK(try_test.errors.empty(), _);
  }

  void parse_tests() {
    TRY(try_test);    
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));
    Func &f(add_func(ctx, "+"));
    add_imp(f, {/*Seq(exe.i64_type)*/}, add);
    //OpSeq ops(compile(ctx, "let foo 35 7 +"));
    //run(ctx, ops);
    //CHECK(get<int64_t>(get_env(ctx, "foo")) == 42, _);
    CHECK(try_test.errors.empty(), _);
  }

  void all_tests() {
    func_tests();
    parse_tests();
  }
}

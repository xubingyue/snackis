#include <iostream>
#include "snabel/exec.hpp"
#include "snabel/op.hpp"
#include "snabel/type.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  static void add_imp(Ctx &ctx, Func &fn) {
    Exec &exe(ctx.coro.exec);
    int64_t res(0);

    for (auto &a: ctx.coro.stack) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(ctx.coro, exe.i64_type, res);
  }
  
  void all_tests() {
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));
    Func &f(add_func(ctx, "+", {}, add_imp)); 
    
    run(ctx,
	{Push(exe.i64_type, int64_t(35)),
	    Push(exe.i64_type, int64_t(7)),
	    Call(f),
	    Push(exe.str_type, str("foo")),
	    Bind()});
    
    CHECK(get<int64_t>(get_env(ctx, "foo")) == 42, _);
  }
}

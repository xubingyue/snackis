#include <iostream>
#include "snabel/exec.hpp"
#include "snabel/op.hpp"
#include "snabel/type.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  static Box add_imp(Ctx &ctx, const std::vector<Box> &args) {
    Exec &exe(ctx.coro.exec);
    int64_t res(0);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res += std::get<int64_t>(a.val);
    }
    
    return Box(exe.i64_type, res);
  }
  
  void all_tests() {
    Exec exe;
    Ctx &ctx(get_ctx(exe.main));
    
    run(ctx,
	{Push(exe.i64_type, int64_t(35)),
	    Push(exe.i64_type, int64_t(7)),
	    Call(add_imp),
	    Push(exe.str_type, str("foo")),
	    Bind()});
    
    CHECK(std::get<int64_t>(get_env(ctx, "foo").val) == 42, _);
  }
}

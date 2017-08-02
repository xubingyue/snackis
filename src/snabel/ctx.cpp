#include "snabel/coro.hpp"
#include "snabel/ctx.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Ctx::Ctx(const Ctx &src):
    coro(src.coro), env(src.env)
  { }

  Ctx::Ctx(Coro &cor):
    coro(cor)
  { }

  opt<Box> find_env(Ctx &ctx, const str &n) {
    auto fnd(ctx.env.find(n));
    if (fnd == ctx.env.end()) { return nullopt; }
    return fnd->second;
  }

  Box get_env(Ctx &ctx, const str &n) {
    auto fnd(find_env(ctx, n));
    CHECK(fnd, _);
    return *fnd;
  }

  void put_env(Ctx &ctx, const str &n, const Box &val) {
    auto fnd(ctx.env.find(n));

    if (fnd != ctx.env.end()) {
      ERROR(Snabel, fmt("Name '%0' is already bound to: %1", n, fnd->second));
    }

    ctx.env.emplace(std::piecewise_construct,
		    std::forward_as_tuple(n),
		    std::forward_as_tuple(val));
  }

  Type &add_type(Ctx &ctx, const str &n) {
    Type &res(ctx.types.emplace_front(n)); 
    put_env(ctx, n, Box(ctx.coro.exec.meta_type, &res));
    return res;
  }

  Func &add_func(Ctx &ctx, const str &n) {
    auto &res(ctx.funcs.emplace_front(n));
    put_env(ctx, n, Box(ctx.coro.exec.func_type, &res));
    return res;
  }
}

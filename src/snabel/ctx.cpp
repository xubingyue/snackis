#include "snabel/ctx.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Ctx::Ctx(Coro &cor, Ctx *par):
    coro(cor), parent(par)
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

  Type &add_type(Ctx &ctx, const str &n) {
    return ctx.types.emplace(std::piecewise_construct,
			     std::forward_as_tuple(n),
			     std::forward_as_tuple(n)).first->second;
  }
}

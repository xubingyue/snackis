#include "snabel/coro.hpp"
#include "snabel/op.hpp"

namespace snabel {
  void run(Ctx &ctx, const Op &op) {
    switch (op.code) {
    case OP_BIND: {
      auto n(get<str>(pop(ctx.coro).val));
      auto fnd(ctx.env.find(n));
      if (fnd != ctx.env.end()) { ctx.env.erase(fnd); }
      auto v(pop(ctx.coro));
      ctx.env.emplace(n, v);
      break;
    }
    case OP_CALL: {
      auto c(std::get<Call>(op.data));
      Box res(c.fn(ctx, ctx.coro.stack));
      push(ctx.coro, res);
      break;
    }
    case OP_PUSH: {
      auto p(std::get<Push>(op.data));
      push(ctx.coro, p.it);
      break;
    }
    default:
      abort();
    }
  }

  void run(Ctx &ctx, const OpStream &ops) {
    for (auto &op: ops) {
      run(ctx, op);
    }
  }
}

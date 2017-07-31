#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/op.hpp"

namespace snabel {
  void run(Ctx &ctx, const Op &op) {
    switch (op.code) {
    case OP_BIND: {
      auto n(get<str>(pop(ctx.coro)));
      auto fnd(ctx.env.find(n));
      if (fnd != ctx.env.end()) { ctx.env.erase(fnd); }
      auto v(pop(ctx.coro));
      ctx.env.emplace(n, v);
      break;
    }
    case OP_CALL: {
      auto c(std::get<Call>(op.data));
      c.fn(ctx);
      break;
    }
    case OP_PUSH: {
      auto p(std::get<Push>(op.data));
      push(ctx.coro, p.type, p.val);
      break;
    }
    default:
      abort();
    }
  }

  void run(Ctx &ctx, const OpSeq &ops) {
    for (auto &op: ops) {
      run(ctx, op);
    }
  }
}

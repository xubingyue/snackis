#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/op.hpp"

namespace snabel {
  void run(Ctx &ctx, const Op &op) {
    switch (op.code) {
    case OP_BIND: {
      auto b(std::get<Bind>(op.data));
      auto v(pop(ctx.coro));
      put_env(ctx, b.name, v);
      break;
    }
    case OP_CALL: {
      auto c(std::get<Call>(op.data));
      Ctx tmp(ctx);
      c.fn(tmp);
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

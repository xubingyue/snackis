#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  void run(Ctx &ctx, const Op &op) {
    switch (op.code) {
    case OP_CALL: {
      auto c(std::get<Call>(op.data));
      
      auto imp(match(c.fn, ctx.coro.stack));

      if (!imp) {
	ERROR(Snabel, fmt("Function not applicable:\n%0", 
			  ctx.coro.stack));
	break;
      }
      
      Ctx tmp(ctx);
      (*imp)(tmp);
      break;
    }
    case OP_ID: {
      auto i(std::get<Id>(op.data));
      auto fnd(find_env(ctx, i.text));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier:\n%0", i.text));
	break;
      }

      push(ctx.coro, *fnd);
      break;
    }
    case OP_LET: {
      auto b(std::get<Let>(op.data));
      auto v(pop(ctx.coro));
      put_env(ctx, b.name, v);
      break;
    }
    case OP_PUSH: {
      auto p(std::get<Push>(op.data));
      push(ctx.coro, *p.type, p.val);
      break;
    }
    case OP_RESET: {
      ctx.coro.stack.clear();
      break;
    }
    default:
      ERROR(Snabel, fmt("Invalid op-code: %0", op.code));
    }
  }

  void run(Ctx &ctx, const OpSeq &ops) {
    for (auto &op: ops) {
      run(ctx, op);
    }
  }
}

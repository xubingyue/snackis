#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  void run(Ctx &ctx, const Op &op) {
    Exec &exe(ctx.coro.exec);
    
    switch (op.code) {
    case OP_BACKUP: {
      backup(ctx.coro);
      break;
    }
    case OP_CALL: {
      auto c(std::get<Call>(op.data));
      call(c.fn, ctx);
      break;
    }
    case OP_ID: {
      auto i(std::get<Id>(op.data));
      auto fnd(find_env(ctx, i.text));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier:\n%0", i.text));
	break;
      }

      if (&fnd->type == &exe.func_type) {
	call(*get<Func *>(*fnd), ctx);
      } else {
	push(ctx.coro, *fnd);
      }
      
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
      ctx.coro.stack->clear();
      break;
    }
    case OP_RESTORE: {
      restore(ctx.coro);
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

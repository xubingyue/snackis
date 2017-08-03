#include "snabel/coro.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Coro::Coro(Exec &exe):
    exec(exe), pc(0), trace_pc(-1), stack(nullptr)
  {
    begin_scope(*this);
  }

  Ctx &get_ctx(Coro &cor) {
    return cor.ctxs.back();
  }

  void push(Coro &cor, const Box &val) {
    cor.stack->push_back(val);
  }

  void push(Coro &cor, Type &typ, const Val &val) {
    cor.stack->emplace_back(typ, val);
  }

  void push(Coro &cor, const Op &op) {
    cor.ops.push_back(op);
  }

  Box pop(Coro &cor) {
    CHECK(!cor.stack->empty(), _);
    auto res(cor.stack->back());
    cor.stack->pop_back();
    return res;
  }

  void stash_stack(Coro &cor) {
    cor.stack = &cor.stacks.emplace_back(Coro::Stack());
  }
  
  void apply_stack(Coro &cor) {
    auto prev(*cor.stack);
    cor.stacks.pop_back();
    CHECK(!cor.stacks.empty(), _);
    cor.stack = &cor.stacks.back();

    if (!prev.empty()) {
      cor.stack->emplace_back(prev.back());
    }
  }

  Ctx &begin_scope(Coro &cor) {    
    stash_stack(cor);

    if (cor.ctxs.empty()) {
      return cor.ctxs.emplace_back(cor);
    }

    return cor.ctxs.emplace_back(cor.ctxs.back());
  }
  
  void end_scope(Coro &cor) {
    apply_stack(cor);
    CHECK(!cor.ctxs.empty(), _);
    cor.ctxs.pop_back();
  }
}

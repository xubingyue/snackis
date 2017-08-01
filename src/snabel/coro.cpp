#include "snabel/coro.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Coro::Coro(Exec &exe):
    exec(exe), pc(0), stack(nullptr)
  {
    ctx.emplace_back(*this);
    stash(*this);
  }

  Ctx &get_ctx(Coro &cor) {
    return cor.ctx.back();
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

  void stash(Coro &cor) {
    cor.stack = &cor.stacks.emplace_back(Coro::Stack());
  }
  
  void apply(Coro &cor) {
    auto prev(*cor.stack);
    cor.stacks.pop_back();
    CHECK(!cor.stacks.empty(), _);
    cor.stack = &cor.stacks.back();
    std::copy(prev.begin(), prev.end(), std::back_inserter(*cor.stack));
  }
}

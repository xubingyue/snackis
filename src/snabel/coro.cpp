#include "snabel/coro.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Coro::Coro(Exec &exe):
    exec(exe), pc(0)
  {
    ctx.emplace_back(*this);
  }

  Ctx &get_ctx(Coro &cor) {
    return cor.ctx.back();
  }

  void push(Coro &cor, const Box &val) {
    cor.stack.push_back(val);
  }

  void push(Coro &cor, Type &typ, const Val &val) {
    cor.stack.emplace_back(typ, val);
  }
  
  Box pop(Coro &cor) {
    CHECK(!cor.stack.empty(), _);
    auto res(cor.stack.back());
    cor.stack.pop_back();
    return res;
  }
}

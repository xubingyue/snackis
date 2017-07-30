#ifndef SNABEL_CORO_HPP
#define SNABEL_CORO_HPP

#include <vector>

#include "snabel/ctx.hpp"
#include "snabel/op.hpp"

namespace snabel {
  struct Exec;
  
  struct Coro {
    Exec &exec;
    int64_t id;
    OpStream ops;
    int64_t pc;
    Ctx root_ctx, *ctx;
    std::vector<Box> stack;
    
    Coro(Exec &exe, int64_t id):
      exec(exe), id(id), pc(0), root_ctx(*this), ctx(&root_ctx)
    { }

    Coro(const Coro &) = delete;
    const Coro &operator =(const Coro &) = delete;
  };

  void push(Coro &cor, const Box &val);
  Box pop(Coro &cor);
}

#endif

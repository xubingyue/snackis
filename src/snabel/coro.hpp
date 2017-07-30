#ifndef SNABEL_CORO_HPP
#define SNABEL_CORO_HPP

#include <vector>

#include "snabel/ctx.hpp"
#include "snabel/op.hpp"

namespace snabel {
  struct Exec;
  
  struct Coro {
    Exec &exec;
    OpStream ops;
    int64_t pc;
    std::vector<Ctx> ctx;
    std::vector<Box> stack;
    
    Coro(Exec &exe);
    Coro(const Coro &) = delete;
    const Coro &operator =(const Coro &) = delete;
  };

  Ctx &get_ctx(Coro &cor);
  void push(Coro &cor, const Box &val);
  Box pop(Coro &cor);
}

#endif

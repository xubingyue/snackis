#ifndef SNABEL_CORO_HPP
#define SNABEL_CORO_HPP

#include <list>

#include "snabel/ctx.hpp"
#include "snabel/op.hpp"

namespace snabel {
  struct Exec;
  
  struct Coro {
    Exec &exec;
    OpSeq ops;
    int64_t pc;
    std::list<Ctx> ctx;
    std::list<Box> stack;
    
    Coro(Exec &exe);
    Coro(const Coro &) = delete;
    const Coro &operator =(const Coro &) = delete;
  };

  Ctx &get_ctx(Coro &cor);
  void push(Coro &cor, const Box &val);
  void push(Coro &cor, Type &typ, const Val &val);
  Box pop(Coro &cor);
}

#endif

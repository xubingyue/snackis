#ifndef SNABEL_CORO_HPP
#define SNABEL_CORO_HPP

#include <deque>
#include <list>

#include "snabel/scope.hpp"
#include "snabel/op.hpp"

namespace snabel {
  struct Exec;
  
  struct Coro {
    using Stack = std::deque<Box>;
    Exec &exec;
    OpSeq ops;
    int64_t pc;
    std::list<Scope> scopes;
    std::list<Stack> stacks;
    Stack *stack;
    
    Coro(Exec &exe);
    Coro(const Coro &) = delete;
    const Coro &operator =(const Coro &) = delete;
  };

  Scope &get_scope(Coro &cor);
  void push(Coro &cor, const Box &val);
  void push(Coro &cor, Type &typ, const Val &val);
  void push(Coro &cor, const Op &op);
  Box pop(Coro &cor);
  void stash_stack(Coro &cor);
  void apply_stack(Coro &cor);
  Scope &begin_scope(Coro &cor);
  void end_scope(Coro &cor);
}

#endif

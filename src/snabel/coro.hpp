#ifndef SNABEL_CORO_HPP
#define SNABEL_CORO_HPP

#include <deque>
#include <list>

#include "snabel/op.hpp"
#include "snabel/scope.hpp"

namespace snabel {
  struct Exec;
  
  using Stack = std::deque<Box>;

  struct Coro {
    Exec &exec;
    OpSeq ops;
    int64_t pc, return_pc;
    std::list<Type> types;
    std::list<Func> funcs;
    std::list<Scope> scopes;
    std::list<Stack> stacks;
    
    Coro(Exec &exe);
    Coro(const Coro &) = delete;
    const Coro &operator =(const Coro &) = delete;
  };

  Scope &curr_scope(Coro &cor);
  const Stack &curr_stack(const Coro &cor);
  Stack &curr_stack(Coro &cor);

  Type &add_type(Coro &cor, const str &n);
  Func &add_func(Coro &cor, const str &n);
  
  void push(Coro &cor, const Box &val);
  void push(Coro &cor, Type &typ, const Val &val);
  opt<Box> peek(Coro &cor);
  Box pop(Coro &cor);

  Stack &backup_stack(Coro &cor, bool copy);
  void restore_stack(Coro &cor);
  
  Scope &begin_scope(Coro &cor, bool copy_stack);
  void end_scope(Coro &cor);

  void jump(Coro &cor, const Label &lbl);
  void rewind(Coro &cor);  
  bool compile(Coro &cor, const str &in);
  void run(Coro &cor); 
}

#endif

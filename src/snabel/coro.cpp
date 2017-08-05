#include "snabel/compiler.hpp"
#include "snabel/coro.hpp"
#include "snabel/exec.hpp"
#include "snabel/parser.hpp"
#include "snackis/core/defer.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Coro::Coro(Exec &exe):
    exec(exe), pc(0)
  {
    begin_scope(*this);
  }

  Scope &curr_scope(Coro &cor) {
    CHECK(!cor.scopes.empty(), _);
    return cor.scopes.back();
  }

  Stack &curr_stack(Coro &cor) {
    CHECK(!cor.stacks.empty(), _);
    return cor.stacks.back();
  }

  const Stack &curr_stack(const Coro &cor) {
    CHECK(!cor.stacks.empty(), _);
    return cor.stacks.back();
  }

  Type &add_type(Coro &cor, const str &n) {
    Type &res(cor.types.emplace_front(n)); 
    put_env(cor.scopes.front(), n, Box(cor.exec.meta_type, &res));
    return res;
  }

  Func &add_func(Coro &cor, const str &n) {
    auto &res(cor.funcs.emplace_front(n));
    put_env(cor.scopes.front(), n, Box(cor.exec.func_type, &res));
    return res;
  }

  void push(Coro &cor, const Box &val) {
    curr_stack(cor).push_back(val);
  }

  void push(Coro &cor, Type &typ, const Val &val) {
    curr_stack(cor).emplace_back(typ, val);
  }

  opt<Box> peek(Coro &cor) {
    auto &s(curr_stack(cor));
    if (s.empty()) { return nullopt; }
    return s.back();
  }

  Box pop(Coro &cor) {
    auto &s(curr_stack(cor));
    CHECK(!s.empty(), _);
    auto res(s.back());
    s.pop_back();
    return res;
  }

  Stack &backup_stack(Coro &cor) {
    return cor.stacks.emplace_back(Stack());
  }
  
  void restore_stack(Coro &cor) {
    CHECK(!cor.stacks.empty(), _);
    auto prev(cor.stacks.back());
    cor.stacks.pop_back();
    CHECK(!cor.stacks.empty(), _);

    if (!prev.empty()) {
      curr_stack(cor).emplace_back(prev.back());
    }
  }

  Scope &begin_scope(Coro &cor) {    
    backup_stack(cor);

    if (cor.scopes.empty()) {
      return cor.scopes.emplace_back(cor);
    }

    return cor.scopes.emplace_back(cor.scopes.back());
  }
  
  void end_scope(Coro &cor) {
    CHECK(!cor.scopes.empty(), _);
    restore_stack(cor);
    cor.scopes.pop_back();
  }

  void rewind(Coro &cor) {
    while (cor.scopes.size() > 1) { cor.scopes.pop_back(); }
    while (cor.stacks.size() > 1) { cor.stacks.pop_back(); }
    curr_stack(cor).clear();

    Scope &scp(curr_scope(cor));
    while (scp.envs.size() > 1) { scp.envs.pop_back(); }
    
    cor.pc = 0;
  }

  bool compile(Coro &cor, const str &in, bool optimize) {
    cor.ops.clear();
    size_t lnr(0);
    
    for (auto &ln: parse_lines(in)) {
      if (!ln.empty()) {
	compile(cor.exec, lnr, parse_expr(ln), cor.ops);
      }
      
      lnr++;
    }

    auto &scp(curr_scope(cor));
    
    while (true) {
      OpSeq out;
      bool done(true);
      cor.pc = 0;
      push_env(scp);
      
      for (auto &op: cor.ops) {
	if (compile(op, curr_scope(cor), optimize, out)) { done = false; }
	cor.pc++;
      }

      cor.ops.clear();
      cor.ops.swap(out);
      pop_env(scp);
      if (done) { break; }
    }

    cor.pc = 0;
    return true;
  }

  void run(Coro &cor) {
    while (cor.pc < cor.ops.size()) {
      run(cor.ops[cor.pc], curr_scope(cor));
      cor.pc++;
    }
  }  
}

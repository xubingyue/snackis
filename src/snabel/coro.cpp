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

  Box peek(Coro &cor) {
    auto &s(curr_stack(cor));
    CHECK(!s.empty(), _);
    return s.back();
  }

  Box pop(Coro &cor) {
    auto &s(curr_stack(cor));
    CHECK(!s.empty(), _);
    auto res(s.back());
    s.pop_back();
    return res;
  }

  Stack &push_stack(Coro &cor) {
    return cor.stacks.emplace_back(Stack());
  }
  
  void pop_stack(Coro &cor) {
    CHECK(!cor.stacks.empty(), _);
    cor.stacks.pop_back();
    CHECK(!cor.stacks.empty(), _);
  }

  Scope &begin_scope(Coro &cor) {    
    push_stack(cor);

    if (cor.scopes.empty()) {
      return cor.scopes.emplace_back(cor);
    }

    return cor.scopes.emplace_back(cor.scopes.back());
  }
  
  void end_scope(Coro &cor) {
    CHECK(!cor.scopes.empty(), _);
    auto prev_stack(curr_stack(cor));
    pop_stack(cor);

    if (!prev_stack.empty()) {
      curr_stack(cor).emplace_back(prev_stack.back());
    }

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

  static bool trace(Coro &cor, bool optimize) {
    OpSeq in;
    cor.ops.swap(in);
    auto &scp(cor.scopes.front());
    bool res(false);
    
    while (true) {
      push_env(scp);
      bool done(true);
      
      for (auto &op: in) {
	if (trace(op, curr_scope(cor), optimize, cor.ops)) { done = false; }
	cor.pc++;
      }

      rewind(cor);
      if (done || !optimize) { break; }
      
      in.clear();
      cor.ops.swap(in);
      res = true;
    }

    return res;
  }

  bool compile(Coro &cor, const str &in, bool optimize) {
    cor.ops.clear();
    size_t lnr(0);
    for (auto &ln: parse_lines(in)) {
      if (!ln.empty()) {
	for (auto &e: parse_exprs(ln)) {
	  compile(cor.exec, lnr, parse_expr(e), cor.ops);
	}
      }
      
      lnr++;
    }

    return trace(cor, optimize);
  }

  bool optimize(Coro &cor) { return trace(cor, true); }

  void run(Coro &cor) {
    while (cor.pc < cor.ops.size()) {
      run(cor.ops[cor.pc], curr_scope(cor));
      cor.pc++;
    }
  }  
}

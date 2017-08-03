#include "snabel/compiler.hpp"
#include "snabel/coro.hpp"
#include "snabel/parser.hpp"
#include "snackis/core/error.hpp"

namespace snabel {
  Coro::Coro(Exec &exe):
    exec(exe), pc(0), stack(nullptr)
  {
    begin_scope(*this);
  }

  Scope &get_scope(Coro &cor) {
    return cor.scopes.back();
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

  Scope &begin_scope(Coro &cor) {    
    stash_stack(cor);

    if (cor.scopes.empty()) {
      return cor.scopes.emplace_back(cor);
    }

    return cor.scopes.emplace_back(cor.scopes.back());
  }
  
  void end_scope(Coro &cor) {
    apply_stack(cor);
    CHECK(!cor.scopes.empty(), _);
    cor.scopes.pop_back();
  }

  void rewind(Coro &cor) {
    cor.stack->clear();
    while (cor.scopes.size() > 1) { cor.scopes.pop_back(); }
    cor.pc = 0;
  }

  static void trace(Coro &cor) {
    OpSeq in;
    in.swap(cor.ops);
    
    while (true) {
      rewind(cor);
      bool done(true);
      
      for (auto &op: in) {
	if (trace(op, get_scope(cor), cor.ops)) { done = false; }
	cor.pc++;
      }
      
      if (done) { break; }
      
      in.clear();
      cor.ops.swap(in);
    }
  }

  void compile(Coro &cor, const str &in) {
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

    trace(cor);
  }

  void run(Coro &cor) {
    rewind(cor);
    begin_scope(cor);
    
    while (cor.pc < cor.ops.size()) {
      run(cor.ops[cor.pc], get_scope(cor));
      cor.pc++;
    }

    end_scope(cor);
  }
  
}

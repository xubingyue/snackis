#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/type.hpp"

namespace snabel {
  static void add_i64(Scope &scp, FuncImp &fn) {
    auto args(get_args(fn, scp));
    Exec &exe(scp.coro.exec);
    int64_t res(0);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res += get<int64_t>(a);
    }
    
    push(scp.coro, exe.i64_type, res);
  }

  static void mul_i64(Scope &scp, FuncImp &fn) {
    auto args(get_args(fn, scp));
    Exec &exe(scp.coro.exec);
    int64_t res(1);

    for (auto &a: args) {
      CHECK(&a.type == &exe.i64_type, _);
      res *= get<int64_t>(a);
    }
    
    push(scp.coro, exe.i64_type, res);
  }
  
  Exec::Exec():
    main(fibers.emplace(std::piecewise_construct,
			std::forward_as_tuple(null_uid),
			std::forward_as_tuple(*this, null_uid)).first->second),
    scope(get_scope(main)),
    meta_type((add_type(scope, "Type"))),
    op_type((add_type(scope, "Op"))),
    op_seq_type((add_type(scope, "OpSeq"))),
    func_type((add_type(scope, "Func"))),
    i64_type((add_type(scope, "I64"))),
    str_type((add_type(scope, "Str")))
  {
    meta_type.fmt = [](auto &v) { return get<Type *>(v)->name; };
    op_type.fmt = [](auto &v) { return fmt_arg(get<Op>(v).code); };
    op_seq_type.fmt = [](auto &v) { return fmt_arg(get<OpSeq>(v).size()); };
    func_type.fmt = [](auto &v) { return fmt_arg(size_t(get<Func *>(v))); };
    i64_type.fmt = [](auto &v) { return fmt_arg(get<int64_t>(v)); };
    str_type.fmt = [](auto &v) { return get<str>(v); };

    Func &add(add_func(scope, "+"));
    add_imp(add, {&i64_type.seq}, add_i64);

    Func &mul(add_func(scope, "*"));
    add_imp(mul, {&i64_type.seq}, mul_i64);
  }

  static void trace(Exec &exe) {
    OpSeq in;
    in.swap(exe.main.ops);
    
    while (true) {
      bool done(true);
      int64_t pc(0);
      
      for (auto &op: in) {
	op.pc = pc;
	if (trace(op, exe.scope, exe.main.ops)) { done = false; }
	pc++;
      }

      if (done) { break; }
      
      in.clear();
      exe.main.ops.swap(in);
    }
  }  
  
  void compile(Exec &exe, const str &in) {
    size_t lnr(0);
    for (auto &ln: parse_lines(in)) {
      if (!ln.empty()) {
	for (auto &e: parse_exprs(ln)) {
	  compile(exe, lnr, parse_expr(e), exe.main.ops);
	}
      }
      
      lnr++;
    }

    trace(exe);
  }

  void run(Exec &exe) {
    Coro &cor(exe.main);
    
    while (cor.pc < cor.ops.size()) {
      run(cor.ops[cor.pc], get_scope(cor));
      cor.pc++;
    }
  }

  void reset(Exec &exe) {
    exe.main.ops.clear();
    rewind(exe);
  }

  void rewind(Exec &exe) {
    Coro &cor(exe.main);
    cor.stack->clear();
    while (cor.scopes.size() > 1) { cor.scopes.pop_back(); }
    exe.main.pc = 0;
  }
}

#include <iostream>

#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  Op Op::make_begin() {
    Op op(OP_BEGIN, "Begin");
    op.run = [](auto &op, auto &scp) { begin_scope(scp.coro); };
    return op;
  }
  
  Op Op::make_call(Func &fn) {
    Op op(OP_CALL, "Call");
    op.info = [&fn](auto &op, auto &scp) { return fn.name; };

    op.run = [&fn](auto &op, auto &scp) {
      auto imp(match(fn, scp.coro));
      
      if (imp) {
	(*imp)(scp.coro);
      } else {
	ERROR(Snabel, fmt("Function not applicable: %0\n%1", 
			  fn.name, curr_stack(scp.coro)));
      }
    };

    return op;
  }

  Op Op::make_drop(size_t cnt) {
    Op op(OP_DROP, "Drop");
    op.info = [cnt](auto &op, auto &scp) { return fmt_arg(cnt); };
    
    op.compile = [cnt](auto &op, auto &scp, bool optimize, auto &out) mutable {
      if (optimize) {
	auto i(0);
	
	while (i < cnt && !out.empty() && out.back().code == OP_PUSH) {
	  out.pop_back();
	  i++;
	}

	if (i == cnt) { return true; }

	if (i) {
	  out.push_back(Op::make_drop(cnt-i));
	  return true;
	}
      }

      return false;
    };

    op.run = [cnt](auto &op, auto &scp) {
      for (size_t i(0); i < cnt; i++) { pop(scp.coro); }
    };
    
    return op;    
  }
  
  Op Op::make_end() {
    Op op(OP_END, "End");
    op.run = [](auto &op, auto &scp) { end_scope(scp.coro); };
    return op;
  }
  
  Op Op::make_id(const str &txt) {
    Op op(OP_ID, "Id");
    op.info = [txt](auto &op, auto &scp) { return txt; };

    op.compile = [txt](auto &op, auto &scp, bool optimize, auto &out) {
	auto fnd(find_env(scp, txt));

	if (!fnd) {
	  ERROR(Snabel, fmt("Unknown identifier: %0", txt));
	  return false;
	}

	if (&fnd->type == &scp.coro.exec.func_type) {
	  Func &fn(*get<Func *>(*fnd));
	  out.push_back(Op::make_call(fn));
	  return true;
	}
	
	return false;
    };

    op.run = [txt](auto &op, auto &scp) {
      auto fnd(find_env(scp, txt));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier: %0", txt));
	return;
      }

      push(scp.coro, *fnd);
    };

    return op;
  }

  Op Op::make_jump(const str &tag, opt<Label> lbl) {
    Op op(OP_JUMP, "Jump");

    op.info = [tag, lbl](auto &op, auto &scp) {
      return fmt("%0 (%1)", tag, lbl ? to_str(lbl->pc) : "?");
    };

    op.compile = [tag, lbl](auto &op, auto &scp, bool optimize, auto &out) {      
      auto fnd(scp.labels.find(tag));
      if (fnd == scp.labels.end()) { return false; }
      if (lbl && fnd->second.pc == lbl->pc) { return false; }
      out.push_back(Op::make_jump(tag, fnd->second));
      return true;
    };

    op.run = [tag, lbl](auto &op, auto &scp) {
      if (lbl) {
	while (scp.coro.scopes.size() > lbl->depth) {
	  scp.coro.scopes.pop_back();
	}

	scp.coro.pc = lbl->pc;
      } else {
	ERROR(Snabel, fmt("Missing label: %0", tag));
      }
    };
    
    return op;
  }
  
  Op Op::make_label(const str &tag) {
    Op op(OP_LABEL, "Label");
    op.info = [tag](auto &op, auto &scp) { return tag; };

    int64_t prev_pc(-1);
    op.compile = [tag, prev_pc](auto &op, auto &scp, bool optimize, auto &out)
      mutable {
      Coro &cor(scp.coro);
      auto fnd(scp.labels.find(tag));

      if (fnd == scp.labels.end()) {
	scp.labels.emplace(std::piecewise_construct,
			   std::forward_as_tuple(tag),
			   std::forward_as_tuple(cor.scopes.size(), cor.pc));
	prev_pc = cor.pc;
	out.push_back(op);
	return true;
      }

      if (prev_pc == cor.pc) { return false; }
      
      if (prev_pc == -1 || fnd->second.pc == prev_pc) {
	fnd->second.depth = cor.scopes.size();
	prev_pc = fnd->second.pc = cor.pc;
      } else {
	ERROR(Snabel, fmt("Duplicate label: %0", tag));
      }
      
      out.push_back(op);
      return true;
    };

    return op;
  }
  
  Op Op::make_let(const str &id) {
    Op op(OP_LET, "Let");
    op.info = [id](auto &op, auto &scp) { return id; };

    int64_t prev_pc(-1);
    op.compile = [id, prev_pc](auto &op, auto &scp, bool optimize, auto &out)
      mutable {
      auto fnd(find_env(scp, id));
      auto &exe(scp.coro.exec);
      
      if (fnd) {
	if (&fnd->type == &exe.i64_type) {
	  if (get<int64_t>(*fnd) != prev_pc) {
	    ERROR(Snabel, fmt("Duplicate binding: %0", id));
	  }
	} else {
	  ERROR(Snabel, fmt("Duplicate binding: %0\n%1", id, *fnd));
	}
      } else {
	put_env(scp, id, Box(exe.i64_type, scp.coro.pc));
	prev_pc = scp.coro.pc;
      }

      return false;
    };

    op.run = [id](auto &op, auto &scp) {
      auto v(pop(scp.coro));
      put_env(scp, id, v);
    };
    
    return op;
  }

  Op Op::make_push(const Box &it) {
    Op op(OP_PUSH, "Push");
    op.info = [it](auto &op, auto &scp) { return fmt_arg(it); };
    op.run = [it](auto &op, auto &scp) { push(scp.coro, it); };
    return op;
  }

  Op Op::make_reset() {
    Op op(OP_RESET, "Reset");
    op.run = [](auto &op, auto &scp) { curr_stack(scp.coro).clear(); };
    return op;
  }

  static str def_info(const Op &op, Scope &scp) { return ""; }

  static bool def_compile(const Op &op, Scope &scp, bool optimize, OpSeq &out) {
    return false;
  }

  static void def_run(const Op &op, Scope &scp)
  { }

  Op::Op(OpCode cod, const str &nam):
    code(cod), name(nam),
    info(def_info), compile(def_compile), run(def_run)
  { }

  str info(const Op &op, Scope &scp) {
    return op.info(op, scp);
  }

  bool compile(const Op &op, Scope &scp, bool optimize, OpSeq &out) {
    if (op.compile(op, scp, optimize, out)) { return true; }
    out.push_back(op);
    return false;
  }

  void run(const Op &op, Scope &scp) {
    op.run(op, scp);
  }
}

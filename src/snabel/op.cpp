#include <iostream>

#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  Op Op::make_apply() {
    Op op(OP_APPLY, "Apply");
    op.run = [](auto &op, auto &scp) { apply_stack(scp.coro); };

    op.trace = [](auto &op, auto &scp, auto &out) {
      apply_stack(scp.coro);
      return false;
    };

    return op;
  }

  Op Op::make_begin() {
    Op op(OP_BEGIN, "Begin");
    op.run = [](auto &op, auto &scp) { begin_scope(scp.coro); };

    op.trace = [](auto &op, auto &scp, auto &out) {
      begin_scope(scp.coro);
      return false;
    };

    return op;
  }
  
  Op Op::make_call(FuncImp &imp) {
    Op op(OP_CALL, "Call");
    op.info = [&imp](auto &op, auto &scp) { return imp.func.name; };

    op.run = [&imp](auto &op, auto &scp) {
      imp(scp.coro);
    };

    op.trace = [&imp](auto &op, auto &scp, auto &out) {
      pop_args(imp, scp.coro);
      push(scp.coro, imp.res_type, undef);
      return false;
    };
    
    return op;
  }

  Op Op::make_end() {
    Op op(OP_END, "End");
    op.run = [](auto &op, auto &scp) { end_scope(scp.coro); };

    op.trace = [](auto &op, auto &scp, auto &out) {
      end_scope(scp.coro);
      return false;
    };

    return op;
  }
  
  Op Op::make_id(const str &txt) {
    Op op(OP_ID, "Id");
    op.info = [txt](auto &op, auto &scp) { return txt; };

    op.run = [txt](auto &op, auto &scp) {
      auto fnd(find_env(scp, txt));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier:\n%0", txt));
	return;
      }

      push(scp.coro, *fnd);
    };    

    op.trace = [txt](auto &op, auto &scp, auto &out) {
	auto fnd(find_env(scp, txt));
	if (!fnd) {return false; }	

	if (undef(*fnd)) {
	  push(scp.coro, *fnd);
	  return false;
	}
	  
	if (&fnd->type == &scp.coro.exec.func_type) {
	  Func &fn(*get<Func *>(*fnd));
	  auto imp(match(fn, scp.coro));

	  if (!imp) {
	    ERROR(Snabel, fmt("Function not applicable:\n%0", 
			      *scp.coro.stack));
	  }

	  snabel::trace(Op::make_call(*imp), scp, out);
	} else {
	  snabel::trace(Op::make_push(*fnd), scp, out);
	}
	
	return true;
    };

    return op;
  }

  static Op make_jump(const str &tag) {
    Op op(OP_JUMP, "Jump");

    op.info = [tag](auto &op, auto &scp) {
      auto fnd(scp.labels.find(tag));
      int64_t pc((fnd == scp.labels.end()) ? -1 : fnd->second.pc);
      return fmt("%0 (%1)", tag, pc);
    };

    op.run = [tag](auto &op, auto &scp) {
      auto fnd(scp.labels.find(tag));
      if (fnd == scp.labels.end()) {
	ERROR(Snabel, fmt("Missing label: %0", tag));
      }

      while (scp.coro.scopes.size() > fnd->second.depth) {
	scp.coro.scopes.pop_back();
      }
      
      scp.coro.pc = fnd->second.pc;
    };

    return op;
  }
  
  Op Op::make_label(const str &tag) {
    Op op(OP_LABEL, "Label");
    op.info = [tag](auto &op, auto &scp) { return tag; };

    int64_t prev_pc(-1);
    op.trace = [tag, prev_pc](auto &op, auto &scp, auto &out) mutable {
      Coro &cor(scp.coro);
      auto fnd(scp.labels.find(tag));

      if (fnd == scp.labels.end()) {
	scp.labels.emplace(std::piecewise_construct,
			   std::forward_as_tuple(tag),
			   std::forward_as_tuple(cor.scopes.size(), cor.pc));
	out.push_back(op);
	prev_pc = cor.pc;
	return true;
      }

      if (prev_pc == -1 || fnd->second.pc == prev_pc) {
	fnd->second.depth = cor.scopes.size();
	prev_pc = fnd->second.pc = cor.pc;
      } else {
	ERROR(Snabel, fmt("Duplicate label: %0", tag));
      }
      
      return false;
    };

    return op;
  }
  
  Op Op::make_let(const str &id) {
    Op op(OP_LET, "Let");
    op.info = [id](auto &op, auto &scp) { return id; };

    op.run = [id](auto &op, auto &scp) {
      auto v(pop(scp.coro));
      put_env(scp, id, v);
    };

    op.trace = [id](auto &op, auto &scp, auto &out) {
      auto v(pop(scp.coro));
      put_env(scp, id, v);
      return false;
    };

    return op;
  }
  
  Op Op::make_push(const Box &it) {
    Op op(OP_PUSH, "Push");
    op.info = [it](auto &op, auto &scp) { return fmt_arg(it); };
    op.run = [it](auto &op, auto &scp) { push(scp.coro, it); };

    op.trace = [it](auto &op, auto &scp, auto &out) {
      push(scp.coro, it);
      return false;
    };

    return op;
  }

  Op Op::make_reset() {
    Op op(OP_RESET, "Reset");
    op.run = [](auto &op, auto &scp) { scp.coro.stack->clear(); };

    op.trace = [](auto &op, auto &scp, auto &out) {
      scp.coro.stack->clear();
      return false;
    };

    return op;
  }
  
  Op Op::make_stash() {
    Op op(OP_STASH, "Stash");
    op.run = [](auto &op, auto &scp) { stash_stack(scp.coro); };

    op.trace = [](auto &op, auto &scp, auto &out) {
      stash_stack(scp.coro);
      return false;
    };

    return op;
  }

  static str def_info(const Op &op, Scope &scp) {
    return "";
  }

  static void def_run(const Op &op, Scope &scp) {
  }

  static bool def_trace(const Op &op, Scope &scp, OpSeq &out) {
    return false;
  }

  Op::Op(OpCode cod, const str &nam):
    code(cod), name(nam),
    info(def_info), run(def_run), trace(def_trace)
  { }

  str info(const Op &op, Scope &scp) {
    return op.info(op, scp);
  }

  bool trace(const Op &op, Scope &scp, OpSeq &out) {
    if (op.trace(op, scp, out)) { return true; }
    out.push_back(op);
    return false;
  }

  void run(const Op &op, Scope &scp) {
    op.run(op, scp);
  }
}

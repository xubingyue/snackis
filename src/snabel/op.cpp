#include <iostream>

#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  Op Op::make_backup() {
    Op op(OP_BACKUP, "Backup");
    op.run = [](auto &op, auto &scp) { backup_stack(scp.coro); };
    return op;
  }

  Op Op::make_begin() {
    Op op(OP_BEGIN, "Begin");
    op.run = [](auto &op, auto &scp) { begin_scope(scp.coro); };
    return op;
  }
  
  Op Op::make_call(FuncImp &imp) {
    Op op(OP_CALL, "Call");
    op.info = [&imp](auto &op, auto &scp) { return imp.func.name; };

    op.run = [&imp](auto &op, auto &scp) {
      imp(scp.coro);
    };

    op.trace = [&imp](auto &op, auto &scp, bool optimize, auto &out) {
      auto args(pop_args(imp, scp.coro));

      if (optimize &&
	  imp.pure &&
	  std::find_if(args.begin(), args.end(),
				   [](auto &a){ return undef(a); }) == args.end()) {
	imp(scp.coro, args);	

	if (!args.empty()) {
	  out.push_back(make_pop(args.size()));
	}

	if (&imp.res_type != &scp.coro.exec.void_type) {
	  auto res(peek(scp.coro));
	  
	  if (res){
	    out.push_back(make_push(*res));
	  } else {
	    ERROR(Snabel, fmt("Missing result from function: %0", imp.func.name));
	  }
	}
	
	return true;
      }

      push(scp.coro, imp.res_type, undef);
      return false;
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

    op.run = [txt](auto &op, auto &scp) {
      auto fnd(find_env(scp, txt));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier: %0", txt));
	return;
      }

      push(scp.coro, *fnd);
    };    

    op.trace = [txt](auto &op, auto &scp, bool optimize, auto &out) {
	auto fnd(find_env(scp, txt));
	if (!fnd) {return false; }

	if (undef(*fnd)) {
	  push(scp.coro, *fnd);
	  return false;
	}

	if (&fnd->type == &scp.coro.exec.func_type) {
	  Func &fn(*get<Func *>(*fnd));
	  auto imp(match(fn, scp.coro));

	  if (imp) {
	    (*imp)(scp.coro);
	    out.push_back(Op::make_call(*imp));
	  } else {
	    ERROR(Snabel, fmt("Function not applicable: %0\n%1", 
			      fn.name, curr_stack(scp.coro)));
	  }	  
	} else {
	  push(scp.coro, *fnd);
	  out.push_back(Op::make_push(*fnd));
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

    op.trace = [tag](auto &op, auto &scp, bool optimize, auto &out) {
      return false;
    };
  }
  
  Op Op::make_label(const str &tag) {
    Op op(OP_LABEL, "Label");
    op.info = [tag](auto &op, auto &scp) { return tag; };

    int64_t prev_pc(-1);
    op.trace = [tag, prev_pc](auto &op, auto &scp, bool optimize, auto &out) mutable {
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

    return op;
  }

  Op Op::make_pop(size_t cnt) {
    Op op(OP_POP, "Pop");
    op.info = [cnt](auto &op, auto &scp) { return fmt_arg(cnt); };
    
    op.run = [cnt](auto &op, auto &scp) {
      for (size_t i(0); i < cnt; i++) { pop(scp.coro); }
    };

    op.trace = [cnt](auto &op, auto &scp, bool optimize, auto &out) mutable {
      if (optimize) {
	while (cnt > 0 && !out.empty() && out.back().code == OP_PUSH) {
	  out.pop_back();
	  cnt--;
	}
      }

      for (size_t i(0); i < cnt; i++) { pop(scp.coro); }
      return !cnt;
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

  Op Op::make_restore() {
    Op op(OP_RESTORE, "Restore");
    op.run = [](auto &op, auto &scp) { restore_stack(scp.coro); };
    return op;
  }
  
  static str def_info(const Op &op, Scope &scp) {
    return "";
  }

  static void def_run(const Op &op, Scope &scp) {
  }

  static bool def_trace(const Op &op, Scope &scp, bool optimize, OpSeq &out) {
    op.run(op, scp);
    return false;
  }

  Op::Op(OpCode cod, const str &nam):
    code(cod), name(nam),
    info(def_info), run(def_run), trace(def_trace)
  { }

  str info(const Op &op, Scope &scp) {
    return op.info(op, scp);
  }

  bool trace(const Op &op, Scope &scp, bool optimize, OpSeq &out) {
    if (op.trace(op, scp, optimize, out)) { return true; }
    out.push_back(op);
    return false;
  }

  void run(const Op &op, Scope &scp) {
    op.run(op, scp);
  }
}

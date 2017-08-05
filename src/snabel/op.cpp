#include <iostream>

#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"
#include "snackis/core/defer.hpp"

namespace snabel {
  Op Op::make_backup(bool copy) {
    Op op(OP_BACKUP);

    op.compile = [](auto &op, auto &scp, auto &out) {
      op.run(op, scp);
      return false;
    };
    
    op.run = [copy](auto &op, auto &scp) { backup_stack(scp.coro, copy); };
    return op;
  }

  Op Op::make_call(Func &fn) {
    Op op(OP_CALL);
    op.info = [&fn](auto &op, auto &scp) { return fn.name; };

    op.compile = [&fn](auto &op, auto &scp, auto &out) mutable {
      auto &s(curr_stack(scp.coro));
      if (s.empty()) { return false; }
      auto imp(match(fn, scp.coro));

      if (imp && imp->pure && !imp->args.empty()) {
	auto args(pop_args(*imp, scp.coro));

	if (std::find_if(args.begin(), args.end(),
			 [](auto &a){ return undef(a); }) == args.end()) {
	  (*imp)(scp.coro, args);
	  out.push_back(Op::make_drop(args.size()));	
	  out.push_back(Op::make_push(pop(scp.coro)));
	  return true;
	}
      }

      if (!imp && fn.imps.size() == 1) { imp = &fn.imps.front(); }

      if (imp) {
	auto args(pop_args(*imp, scp.coro));

	if (&imp->res_type != &scp.coro.exec.void_type) {
	  push(scp.coro, Box(imp->res_type, undef));
	}
	
	return false;
      }

      s.clear();
      return false;
    };
    
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
    Op op(OP_DROP);
    op.info = [cnt](auto &op, auto &scp) { return fmt_arg(cnt); };
    
    op.compile = [cnt](auto &op, auto &scp, auto &out) mutable {
      Stack &s(curr_stack(scp.coro));
      for (size_t i(0); i < cnt && !s.empty(); i++) {
	s.pop_back();
      }
      
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

      return false;
    };

    op.run = [cnt](auto &op, auto &scp) {
      for (size_t i(0); i < cnt; i++) { pop(scp.coro); }
    };
    
    return op;    
  }

  Op Op::make_dyncall(opt<Label> lbl) {
    Op op(OP_DYNCALL);

    op.info = [lbl](auto &op, auto &scp) {
      return lbl ? lbl->tag : "";
    };

    op.compile = [lbl](auto &op, auto &scp, auto &out) mutable {
      auto &cor(scp.coro);
      auto &exe(cor.exec);      
      DEFER({ curr_stack(cor).clear(); });
      auto fn(peek(cor));
      
      if (fn && &fn->type == &exe.lambda_type) {
	auto fnd(scp.labels.find(get<str>(*fn)));
	
	if (fnd != scp.labels.end()) {
	  out.push_back(Op::make_drop(1));
	  out.push_back(Op::make_dyncall(fnd->second));
	  return true;
	}
      }
      
      return false;
    };

    op.run = [lbl](auto &op, auto &scp) mutable {
      auto &cor(scp.coro);
      cor.returns.push_back(cor.pc);

      if (!lbl) {
	auto fn(pop(cor));
	str n(get<str>(fn));
	auto fnd(scp.labels.find(n));

	if (fnd == scp.labels.end()) {
	  ERROR(Snabel, fmt("Missing dyncall target: %0", n));
	} else {
	  lbl.emplace(fnd->second);
	}      
      }

      if (lbl) { jump(cor, *lbl); }
    };
    
    return op;    
  }
    
  Op Op::make_group(bool copy_stack) {
    Op op(OP_GROUP);

    op.info = [copy_stack](auto &op, auto &scp) {
      return copy_stack ? "copy" : "empty";
    };

    op.compile = [](auto &op, auto &scp, auto &out) mutable {
      op.run(op, scp);
      return false;
    };
    
    op.run = [copy_stack](auto &op, auto &scp) { begin_scope(scp.coro, copy_stack); };
    return op;
  }

  Op Op::make_id(const str &txt) {
    Op op(OP_ID);
    op.info = [txt](auto &op, auto &scp) { return txt; };

    op.compile = [txt](auto &op, auto &scp, auto &out) {
	auto fnd(find_env(scp, txt));

	if (!fnd) {
	  ERROR(Snabel, fmt("Unknown identifier: %0", txt));
	  return false;
	}

	if (&fnd->type == &scp.coro.exec.func_type) {
	  Func &fn(*get<Func *>(*fnd));
	  out.push_back(Op::make_call(fn));
	  curr_stack(scp.coro).clear();
	  return true;
	}

	if (&fnd->type != &scp.coro.exec.undef_type &&
	    &fnd->type != &scp.coro.exec.void_type) {
	  push(scp.coro, *fnd);
	  out.push_back(Op::make_push(*fnd));
	  return true;
	}

	curr_stack(scp.coro).clear();
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
    Op op(OP_JUMP);

    op.info = [tag, lbl](auto &op, auto &scp) {
      return fmt("%0 (%1)", tag, lbl ? to_str(lbl->pc) : "?");
    };

    op.compile = [tag, lbl](auto &op, auto &scp, auto &out) {      
      curr_stack(scp.coro).clear();
      auto fnd(scp.labels.find(tag));
      if (fnd == scp.labels.end()) { return false; }
      if (lbl && fnd->second.pc == lbl->pc) { return false; }
      out.push_back(Op::make_jump(tag, fnd->second));
      return true;
    };

    op.run = [tag, lbl](auto &op, auto &scp) {
      if (lbl) {
	jump(scp.coro, *lbl);
      } else {
	ERROR(Snabel, fmt("Missing label: %0", tag));
      }
    };
    
    return op;
  }
  
  Op Op::make_label(const str &tag) {
    Op op(OP_LABEL);
    op.info = [tag](auto &op, auto &scp) { return tag; };

    int64_t prev_pc(-1);
    op.compile = [tag, prev_pc](auto &op, auto &scp, auto &out)
      mutable {
      Coro &cor(scp.coro);
      auto fnd(scp.labels.find(tag));

      if (fnd == scp.labels.end()) {
	scp.labels.emplace(std::piecewise_construct,
			   std::forward_as_tuple(tag),
			   std::forward_as_tuple(tag, cor.scopes.size(), cor.pc));
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

  Op Op::make_lambda() {
    Op op(OP_LAMBDA);

    op.compile = [](auto &op, auto &scp, auto &out) mutable {
      Coro &cor(scp.coro);
      curr_stack(cor).clear();
      
      const Sym tag(gensym(cor.exec));
      scp.lambdas.push_back(tag);
      out.push_back(Op::make_jump(fmt("_exit%0", tag)));
      out.push_back(Op::make_label(fmt("_enter%0", tag)));
      out.push_back(Op::make_group(true));
      return true;
    };
    
    return op;
  }
  
  Op Op::make_let(const str &id) {
    Op op(OP_LET);
    op.info = [id](auto &op, auto &scp) { return id; };

    int64_t prev_pc(-1);
    op.compile = [id, prev_pc](auto &op, auto &scp, auto &out)
      mutable {
      auto fnd(find_env(scp, id));
      auto &exe(scp.coro.exec);
      
      if (fnd && &fnd->type != &exe.undef_type) {
	if (&fnd->type == &exe.void_type) {
	  if (get<int64_t>(*fnd) != prev_pc) {
	    ERROR(Snabel, fmt("Duplicate binding: %0", id));
	  }
	} else {
	  ERROR(Snabel, fmt("Duplicate binding: %0\n%1", id, *fnd));
	}

	auto &s(curr_stack(scp.coro));

	if (s.size() == 1) {
	  s.pop_back();
	} else {
	  s.clear();
	}
      } else {
	opt<Box> val;	
	  auto &s(curr_stack(scp.coro));

	if (s.size() == 1) {
	  val.emplace(s.back());
	  s.pop_back();
	} else {
	  s.clear();
	}
	
	put_env(scp, id, val ? *val : Box(exe.void_type, scp.coro.pc));
	prev_pc = scp.coro.pc;
      }

      return false;
    };

    op.run = [id](auto &op, auto &scp) {
      auto &s(curr_stack(scp.coro));
      
      if (s.size() == 1) {
	auto v(s.back());
	s.pop_back();
	put_env(scp, id, v);
      } else {
	ERROR(Snabel, fmt("Malformed run binding: %0\n%1", id, s));
      }
    };
    
    return op;
  }

  Op Op::make_push(const Box &it) {
    Op op(OP_PUSH);
    op.info = [it](auto &op, auto &scp) { return fmt_arg(it); };
    
    op.compile = [it](auto &op, auto &scp, auto &out) {
      op.run(op, scp);
      return false;
    };
    
    op.run = [it](auto &op, auto &scp) { push(scp.coro, it); };
    return op;
  }

  Op Op::make_reset() {
    Op op(OP_RESET);

    op.compile = [](auto &op, auto &scp, auto &out) {
      op.run(op, scp);
      return false;
    };
    
    op.run = [](auto &op, auto &scp) { curr_stack(scp.coro).clear(); };
    return op;
  }

  Op Op::make_restore() {
    Op op(OP_RESTORE);

    op.compile = [](auto &op, auto &scp, auto &out) {
      op.run(op, scp);
      return false;
    };
    
    op.run = [](auto &op, auto &scp) { restore_stack(scp.coro); };
    return op;
  }

  Op Op::make_return() {
    Op op(OP_RETURN);

    op.run = [](auto &op, auto &scp) {
      Coro &cor(scp.coro);
      cor.pc = cor.returns.back();
      cor.returns.pop_back();
    };
    
    return op;
  }

  Op Op::make_ungroup() {
    Op op(OP_UNGROUP);

    op.compile = [](auto &op, auto &scp, auto &out) mutable {
      auto &cor(scp.coro);
      bool clear(curr_stack(cor).empty());
      op.run(op, scp);
      if (clear) { curr_stack(cor).clear(); }
      return false;
    };

    op.run = [](auto &op, auto &scp) { end_scope(scp.coro); };
    return op;
  }

  Op Op::make_unlambda() {
    Op op(OP_UNLAMBDA);

    op.compile = [](auto &op, auto &scp, auto &out) mutable {
      Coro &cor(scp.coro);
      curr_stack(cor).clear();
      
      if (scp.lambdas.empty()) {
	ERROR(Snabel, "Missing lambda start");
	return false;
      }

      const Sym tag(scp.lambdas.back());
      scp.lambdas.pop_back();
      out.push_back(Op::make_ungroup());
      out.push_back(Op::make_return());
      out.push_back(Op::make_label(fmt("_exit%0", tag)));
      out.push_back(Op::make_push(Box(cor.exec.lambda_type, fmt("_enter%0", tag))));
      return true;
    };
    
    return op;
  }
  
  static str def_info(const Op &op, Scope &scp) { return ""; }

  static bool def_compile(const Op &op, Scope &scp, OpSeq &out) {
    return false;
  }

  static void def_run(const Op &op, Scope &scp)
  { }

  Op::Op(OpCode cod):
    code(cod),
    info(def_info), compile(def_compile), run(def_run)
  { }

  str name(const Op &op) {
    switch (op.code){
    case OP_BACKUP:
      return "Backup";
    case OP_CALL:
      return "Call";
    case OP_DROP:
      return "Drop";
    case OP_DYNCALL:
      return "Dyncall";
    case OP_GROUP:
      return "Group";
    case OP_ID:
      return "Id";
    case OP_JUMP:
      return "Jump";
    case OP_LABEL:
      return "Label";
    case OP_LAMBDA:
      return "Lambda";
    case OP_LET:
      return "Let";
    case OP_PUSH:
      return "Push";
    case OP_RESET:
      return "Reset";
    case OP_RESTORE:
      return "Restore";
    case OP_RETURN:
      return "Return";
    case OP_UNGROUP:
      return "Ungroup";
    case OP_UNLAMBDA:
      return "Unlambda";
    };

    ERROR(Snabel, fmt("Invalid op code: %0", op.code));
    return "n/a";
  }

  
  str info(const Op &op, Scope &scp) {
    return op.info(op, scp);
  }

  bool compile(const Op &op, Scope &scp, OpSeq &out) {
    if (op.compile(op, scp, out)) { return true; }
    out.push_back(op);
    return false;
  }

  void run(const Op &op, Scope &scp) {
    op.run(op, scp);
  }
}

#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  Op Op::make_apply() {
    Op op(OP_APPLY, "Apply");
    op.run = [](auto &ctx) { apply_stack(ctx.coro); };
    return op;
  }

  Op Op::make_begin() {
    Op op(OP_BEGIN, "Begin");
    op.run = [](auto &ctx) { begin_scope(ctx.coro); };
    return op;
  }
  
  Op Op::make_call(Func &fn) {
    Op op(OP_CALL, "Call");
    op.info = [&fn]() { return fn.name; };
    op.run = [&fn](auto &ctx) { call(fn, ctx); };
    return op;
  }

  Op Op::make_end() {
    Op op(OP_END, "End");
    op.run = [](auto &ctx) { end_scope(ctx.coro); };
    return op;
  }
  
  Op Op::make_id(const str &txt) {
    Op op(OP_ID, "Id");
    op.info = [txt]() { return txt; };
    op.run = [txt](auto &ctx) {
      auto fnd(find_env(ctx, txt));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier:\n%0", txt));
	return;
      }

      if (&fnd->type == &ctx.coro.exec.func_type) {
	call(*get<Func *>(*fnd), ctx);
      } else {
	push(ctx.coro, *fnd);
      }
    };    

    op.trace = [txt](auto &op, auto &ctx, auto &out) {
	auto fnd(find_env(ctx, txt));

	if (fnd) {
	  if (&fnd->type == &ctx.coro.exec.func_type) {
	    out.emplace_back(Op::make_call(*get<Func *>(*fnd)));
	  } else {
	    out.emplace_back(Op::make_push(*fnd));
	  }
	} else {
	  out.emplace_back(op);
	}
    };

    return op;
  }

  Op Op::make_let(const str &id) {
    Op op(OP_LET, "Let");
    op.info = [id]() { return id; };
    op.run = [id](auto &ctx) {
      auto v(pop(ctx.coro));
      put_env(ctx, id, v);
    };
    return op;
  }
  
  Op Op::make_push(const Box &it) {
    Op op(OP_PUSH, "Push");
    op.info = [it]() { return fmt_arg(it); };
    op.run = [it](auto &ctx) { push(ctx.coro, it); };
    return op;
  }

  Op Op::make_reset() {
    Op op(OP_RESET, "Reset");
    op.run = [](auto &ctx) { ctx.coro.stack->clear(); };
    return op;
  }
  
  Op Op::make_stash() {
    Op op(OP_STASH, "Stash");
    op.run = [](auto &ctx) { stash_stack(ctx.coro); };
    return op;
  }

  static str def_info() {
    return "";
  }

  static void def_run(Ctx &ctx) {
    CHECK(false, _);
  }

  Op::Op(OpCode cod, const str &nam):
    code(cod), name(nam),
    info(def_info), run(def_run),
    trace([](auto &op, auto &ctx, auto &out) { out.emplace_back(op); })
  { }
}

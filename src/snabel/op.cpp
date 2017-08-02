#include "snabel/box.hpp"
#include "snabel/coro.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/func.hpp"
#include "snabel/op.hpp"

namespace snabel {
  BasicOp::BasicOp(const str &nam, const str &inf):
    name(nam), info(inf)
  { }
    
  Apply::Apply():
    BasicOp("Apply")
  { }

  Begin::Begin():
    BasicOp("Begin")
  { }

  Call::Call(Func &fn):
    BasicOp("Call", fmt("#%0", to_str(fn.id).substr(0, 8))),
    fn(fn)
  { }

  End::End():
    BasicOp("End")
  { }

  Id::Id(const str &txt):
    BasicOp("Id", txt),
    text(txt)
  { }

  Let::Let(const str &n):
    BasicOp("Let", n),
    name(n)
  { }
    
  Push::Push(Type &t, const Val &v):
    BasicOp("Push", fmt_arg(Box(t, v))),
    type(&t), val(v)
  { }
  
  Push::Push(const Push &src):
    BasicOp(src),
    type(src.type), val(src.val)
  { }
  
  const Push &Push::operator=(const Push &src) {
    name = src.name;
    info = src.info;
    type = src.type;
    val = src.val;
    return *this;
  }

  Reset::Reset():
    BasicOp("Reset")
  { }

  Stash::Stash():
    BasicOp("Stash")
  { }

  Op::Op(OpCode cod, const OpData &dat):
    code(cod), data(dat)
  { }

  Op::Op(const Apply &dat): Op(OP_APPLY, dat)
  { }
  
  Op::Op(const Begin &dat): Op(OP_BEGIN, dat)
  { }
  
  Op::Op(const Call &dat): Op(OP_CALL, dat)
  { }
  
  Op::Op(const End &dat): Op(OP_END, dat)
  { }
  
  Op::Op(const Id &dat): Op(OP_ID, dat)
  { }
  
  Op::Op(const Let &dat): Op(OP_LET, dat)
  { }
  
  Op::Op(const Push &dat): Op(OP_PUSH, dat)
  { }

  Op::Op(const Reset &dat): Op(OP_RESET, dat)
  { }
  
  Op::Op(const Stash &dat): Op(OP_STASH, dat)
  { }
  
  void run(const Op &op, Ctx &ctx) {
    Exec &exe(ctx.coro.exec);
    
    switch (op.code) {
    case OP_APPLY: {
      apply_stack(ctx.coro);
      break;
    }
    case OP_BEGIN: {
      begin_scope(ctx.coro);
      break;
    }
    case OP_CALL: {
      auto c(std::get<Call>(op.data));
      call(c.fn, ctx);
      break;
    }
    case OP_END: {
      end_scope(ctx.coro);
      break;
    }
    case OP_ID: {
      auto i(std::get<Id>(op.data));
      auto fnd(find_env(ctx, i.text));

      if (!fnd) {
	ERROR(Snabel, fmt("Unknown identifier:\n%0", i.text));
	break;
      }

      if (&fnd->type == &exe.func_type) {
	call(*get<Func *>(*fnd), ctx);
      } else {
	push(ctx.coro, *fnd);
      }
      
      break;
    }
    case OP_LET: {
      auto b(std::get<Let>(op.data));
      auto v(pop(ctx.coro));
      put_env(ctx, b.name, v);
      break;
    }
    case OP_PUSH: {
      auto p(std::get<Push>(op.data));
      push(ctx.coro, *p.type, p.val);
      break;
    }
    case OP_RESET: {
      ctx.coro.stack->clear();
      break;
    }
    case OP_STASH: {
      stash_stack(ctx.coro);
      break;
    }
    default:
      ERROR(Snabel, fmt("Invalid op-code: %0", op.code));
    }
  }
}

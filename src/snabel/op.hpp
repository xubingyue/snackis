#ifndef SNABEL_OP_HPP
#define SNABEL_OP_HPP

#include <utility>
#include <variant>

#include "snabel/func.hpp"
#include "snabel/type.hpp"
#include "snackis/core/func.hpp"

namespace snabel {
  using namespace snackis;

  struct Box;
  struct Coro;
  struct Ctx;

  enum OpCode { OP_BIND, OP_CALL, OP_PUSH };

  struct Op;
  
  using OpSeq = std::vector<Op>;

  struct Bind {
    const str name;
    
    Bind(const str &n):
      name(n)
    { }
  };
    
  struct Call {
    FuncImp &fn;

    Call(FuncImp &fn):
      fn(fn)
    { }
  };

  using Val = std::variant<int64_t, str, Func *, Type *>;
  
  struct Push {
    Type &type;
    Val val;
    
    Push(Type &t, const Val &v):
      type(t), val(v)
    { }

    Push(const Push &p):
      type(p.type), val(p.val)
    { }
  };
  
  using OpData = std::variant<Bind, Call, Push>;

  struct Op {
    OpCode code;
    OpData data;
    
    Op(OpCode cod, const OpData &dat):
      code(cod), data(dat)
    { }

    Op(const Bind &dat): Op(OP_BIND, OpData(dat))
    { }

    Op(const Call &dat): Op(OP_CALL, dat)
    { }

    Op(const Push &dat): Op(OP_PUSH, dat)
    { }
  };
  
  void run(Ctx &ctx, const Op &op);
  void run(Ctx &ctx, const OpSeq &ops);
}

#endif

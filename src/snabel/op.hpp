#ifndef SNABEL_OP_HPP
#define SNABEL_OP_HPP

#include <utility>
#include <variant>

#include "snabel/box.hpp"
#include "snackis/core/func.hpp"

namespace snabel {
  struct Coro;

  enum OpCode { OP_BIND, OP_CALL, OP_PUSH };

  struct Op;
  
  using OpStream = std::vector<Op>;

  struct Bind
  { };
    
  struct Call {
    using Fn = func<Box (Ctx &ctx, const std::vector<Box> &)>;
    Fn fn;

    Call(Fn fn):
      fn(fn)
    { }
  };

  struct Push {
    Box it;
    
    Push(Type &t, const Val &v):
      it(t, v)
    { }
  };
  
  using OpData = std::variant<Bind, Call, Push>;

  struct Op {
    OpCode code;
    OpData data;
    
    Op(OpCode cod, const OpData &dat):
      code(cod), data(dat)
    { }

    Op(const Bind &dat): Op(OP_BIND, dat)
    { }

    Op(const Call &dat): Op(OP_CALL, dat)
    { }

    Op(const Push &dat): Op(OP_PUSH, dat)
    { }
  };
  
  void run(Ctx &ctx, const Op &op);
  void run(Ctx &ctx, const OpStream &ops);
}

#endif

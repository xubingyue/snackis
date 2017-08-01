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

  enum OpCode { OP_APPLY, OP_CALL, OP_ID, OP_LET, OP_PUSH, OP_RESET, OP_STASH};

  struct Op;
  
  using OpSeq = std::vector<Op>;

  struct Apply
  { };

  struct Call {
    Func fn;

    Call(Func &fn):
      fn(fn)
    { }
  };

  struct Id {
    str text;

    Id(const str &txt):
      text(txt)
    { }
  };

  struct Let {
    const str name;
    
    Let(const str &n):
      name(n)
    { }
  };
    
  using Val = std::variant<int64_t, str, Func *, Type *>;
  
  struct Push {
    Type *type;
    Val val;
    
    Push(Type &t, const Val &v):
      type(&t), val(v)
    { }

    Push(const Push &src):
      type(src.type), val(src.val)
    { }

    const Push &operator=(const Push &src) {
      type = src.type;
      val = src.val;
      return *this;
    }
  };

  struct Reset
  { };

  struct Stash
  { };

  using OpData = std::variant<Apply, Call, Id, Let, Push, Reset, Stash>;

  struct Op {
    OpCode code;
    OpData data;
    
    Op(OpCode cod, const OpData &dat):
      code(cod), data(dat)
    { }

    Op(const Apply &dat): Op(OP_APPLY, dat)
    { }

    Op(const Call &dat): Op(OP_CALL, dat)
    { }

    Op(const Id &dat): Op(OP_ID, OpData(dat))
    { }

    Op(const Let &dat): Op(OP_LET, OpData(dat))
    { }

    Op(const Push &dat): Op(OP_PUSH, dat)
    { }

    Op(const Reset &dat): Op(OP_RESET, dat)
    { }

    Op(const Stash &dat): Op(OP_STASH, dat)
    { }
  };
  
  void run(Ctx &ctx, const Op &op);
  void run(Ctx &ctx, const OpSeq &ops);
}

#endif

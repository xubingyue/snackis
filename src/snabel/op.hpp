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

  enum OpCode { OP_APPLY, OP_BEGIN, OP_CALL, OP_END, OP_ID, OP_LET, OP_PUSH,
		OP_RESET, OP_STASH};

  struct Op;
  
  using OpSeq = std::vector<Op>;

  struct Apply {
    Apply();
  };

  struct Begin {
    Begin();
  };

  struct Call {
    Func fn;

    Call(Func &fn);
  };

  struct End {
    End();
  };

  struct Id {
    str text;

    Id(const str &txt);
  };

  struct Let {
    const str name;
    
    Let(const str &n);
  };
    
  using Val = std::variant<int64_t, str, Func *, Type *>;
  
  struct Push {
    Type *type;
    Val val;
    
    Push(Type &t, const Val &v);
    Push(const Push &src);
    const Push &operator=(const Push &src);
  };

  struct Reset {
    Reset();
  };

  struct Stash {
    Stash();
  };

  using OpData = std::variant<Apply, Begin, Call, End, Id, Let, Push, Reset, Stash>;

  struct Op {
    OpCode code;
    OpData data;
    
    Op(OpCode cod, const OpData &dat);
    Op(const Apply &dat);
    Op(const Begin &dat);
    Op(const Call &dat);
    Op(const End &dat);
    Op(const Id &dat);
    Op(const Let &dat);
    Op(const Push &dat);
    Op(const Reset &dat);
    Op(const Stash &dat);
  };
  
  void run(const Op &op, Ctx &ctx);
  str name(const Op &op);
  str info(const Op &op);
}

#endif

#ifndef SNABEL_OP_HPP
#define SNABEL_OP_HPP

#include <deque>
#include <utility>

#include "snackis/core/func.hpp"
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;

  struct Box;
  struct Scope;
  struct Func;
  struct Op;

  enum OpCode { OP_APPLY, OP_BEGIN, OP_CALL, OP_END, OP_ID, OP_JUMP, OP_LABEL,
		OP_LET, OP_PUSH, OP_RESET, OP_STASH};

  using OpSeq = std::deque<Op>;

  struct Op {
    OpCode code;
    str name;
    
    func<str (const Op &op, Scope &)> info;
    func<void (const Op &op, Scope &)> run;
    func<bool (const Op &op, Scope &, OpSeq &)> trace;
    
    static Op make_apply();
    static Op make_begin();
    static Op make_call(Func &fn);
    static Op make_end();
    static Op make_id(const str &txt);
    static Op make_jump(const str &tag);
    static Op make_label(const str &tag);
    static Op make_let(const str &id);
    static Op make_push(const Box &it);
    static Op make_reset();
    static Op make_stash();

    Op(OpCode cod, const str &nam);
  };

  str info(const Op &op, Scope &scp);
  bool trace(const Op &op, Scope &scp, OpSeq &out);
  void run(const Op &op, Scope &scp);
}

#endif

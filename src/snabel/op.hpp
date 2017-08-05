#ifndef SNABEL_OP_HPP
#define SNABEL_OP_HPP

#include <deque>
#include <utility>

#include "snabel/label.hpp"
#include "snackis/core/func.hpp"
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;

  struct Box;
  struct Func;
  struct Scope;
  struct Op;

  enum OpCode { OP_BACKUP, OP_BEGIN, OP_BEGIN_LAMBDA, OP_CALL, OP_DROP, OP_DYNCALL,
	        OP_END, OP_END_LAMBDA, OP_ID, OP_JUMP, OP_LABEL, OP_LET,
		OP_PUSH, OP_RESET, OP_RESTORE, OP_RETURN };

  using OpSeq = std::deque<Op>;

  struct Op {
    OpCode code;
    
    func<str (const Op &op, Scope &)> info;
    func<bool (const Op &op, Scope &, OpSeq &)> compile;
    func<void (const Op &op, Scope &)> run;
    
    static Op make_backup(bool copy);
    static Op make_begin(bool copy_stack);
    static Op make_begin_lambda();
    static Op make_call(Func &fn);
    static Op make_drop(size_t cnt=1);
    static Op make_dyncall(opt<Label> lbl=nullopt);
    static Op make_end();
    static Op make_end_lambda();
    static Op make_id(const str &txt);
    static Op make_jump(const str &tag, opt<Label> lbl=nullopt);
    static Op make_label(const str &tag);
    static Op make_let(const str &id);
    static Op make_push(const Box &it);
    static Op make_reset();
    static Op make_restore();
    static Op make_return();

    Op(OpCode cod);
  };

  str name(const Op &op);
  str info(const Op &op, Scope &scp);
  bool compile(const Op &op, Scope &scp, OpSeq &out);
  void run(const Op &op, Scope &scp);
}

#endif

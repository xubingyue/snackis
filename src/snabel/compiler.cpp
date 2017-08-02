#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"
#include "snabel/parser.hpp"

namespace snabel {  
  Compiler::Compiler(Ctx &ctx):
    ctx(ctx)
  { }

  static void trace(Ctx &ctx, OpSeq &ops) {
    Exec &exe(ctx.coro.exec);
    
    for (auto &op: ops) {
      switch (op.code) {
      case OP_APPLY:
      case OP_BEGIN:
      case OP_CALL:
      case OP_END:
	break;
      case OP_ID: {
	auto i(std::get<Id>(op.data));
	auto fnd(find_env(ctx, i.text));

	if (fnd) {
	  if (&fnd->type == &exe.func_type) {
	    op.code = OP_CALL;
	    op.data = Call(*get<Func *>(*fnd));
	  } else {
	    op.code = OP_PUSH;
	    op.data = Push(fnd->type, std::get<Val>(fnd->val));
	  }
	}
	break;
      }
      case OP_LET:
      case OP_PUSH:
      case OP_RESET:
      case OP_STASH:
	break;
      default:
	ERROR(Snabel, fmt("Invalid op-code: %0", op.code));
      }
    }
  }

  void compile(Compiler &cpr,
	       size_t lnr,
	       const Tok &tok,
	       OpSeq &out) {
    Ctx &ctx(cpr.ctx);
    Exec &exe(ctx.coro.exec);

    if (tok.text[0] == '(') {
      out.emplace_back(Begin());

      for (auto &e: parse_exprs(tok.text.substr(1, tok.text.size()-2))) {
	compile(cpr, lnr, parse_expr(e), out);
      }
      
      out.emplace_back(End());
    } else if (isdigit(tok.text[0]) || 
	(tok.text.size() > 1 && tok.text[0] == '-' && isdigit(tok.text[1]))) {
      out.emplace_back(Push(exe.i64_type, to_int64(tok.text)));
    } else if (tok.text == "apply") {
      out.emplace_back(Apply());
    } else if (tok.text == "begin") {
      out.emplace_back(Begin());
    } else if (tok.text == "end") {
      out.emplace_back(End());
    } else if (tok.text == "reset") {
      out.emplace_back(Reset());
    } else if (tok.text == "stash") {
      out.emplace_back(Stash());
    } else {
      out.emplace_back(Id(tok.text));
    }
  }

  void compile(Compiler &cpr,
	       size_t lnr,
	       const std::vector<Tok> &exp,
	       OpSeq &out) {
    if (exp.empty()) { return; }
    OpSeq ops;
    
    if (exp[0].text == "let") {
      if (exp.size() < 3) {
	ERROR(Snabel, fmt("Malformed let statement in line %0", lnr));
      } else {
	compile(cpr, lnr,
		     std::vector<Tok>(std::next(exp.begin(), 2),
				      exp.end()),
		     ops);

	ops.emplace_back(Let(exp[1].text));
      }
    } else {
      for (auto t: exp) { compile(cpr, lnr, t, ops); }	  
    }

    trace(cpr.ctx, ops);
    std::copy(ops.begin(), ops.end(),
	      std::back_inserter(out));
  }

  
  void compile(Compiler &cpr, const str &in) {
    size_t lnr(0);
    
    for (auto &ln: parse_lines(in)) {
      if (!ln.empty()) {
	for (auto &e: parse_exprs(ln)) {
	  compile(cpr, lnr, parse_expr(e), cpr.ops);
	}
      }
      
      lnr++;
    }
  }
}

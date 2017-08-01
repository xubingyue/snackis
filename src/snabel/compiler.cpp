#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"

namespace snabel {
  struct Tok {
    str text;
    size_t i, j;
    
    Tok(const str &txt, size_t i, size_t j):
      text(txt),
      i(i), j(j)
    { }
  };
  
  Compiler::Compiler(Ctx &ctx):
    ctx(ctx)
  { }

  static std::vector<str> split_lines(const str &in) {
    size_t i(0), j;
    std::vector<str> out;
    
    while ((j=in.find('\n', i)) != str::npos) {
      if (j > i && (i == 0 || in[i-1] == '\\')) {
	out.push_back(trim(in.substr(i, j-i)));
      }

      i = j+1;
    }

    if (i < in.size()) { out.push_back(trim(in.substr(i))); }
    return out;
  }

  static std::vector<Tok> parse_expr(const str &in) {
    size_t i(0);
    bool quoted(false);
    std::vector<Tok> out;
    
    for (size_t j(0); j < in.size(); j++) {
      auto &c(in[j]);
      switch(c) {
      case '"':
	if (j == 0 || in[j-1] != '\\') { quoted = !quoted; }
	break;
      case ' ':
	if (j > i && !quoted) {
	  out.emplace_back(in.substr(i, j-i), i, j);
	  i = j+1;
	}
	
	break;
      }

      if (j == in.size()-1 && (j > i || c != ' ')) {
	out.emplace_back(in.substr(i, j-i+1), i, j);
      }
    }

    return out;
  }

  static void compile_tok(Compiler &cpr,
			  size_t lnr,
			  const Tok &tok,
			  OpSeq &out) {
    Ctx &ctx(cpr.ctx);
    Exec &exe(ctx.coro.exec);
    
    if (isdigit(tok.text[0]) || 
	(tok.text.size() > 1 && tok.text[0] == '-' && isdigit(tok.text[1]))) {
      out.emplace_back(Push(exe.i64_type, to_int64(tok.text)));
    } else {
      out.emplace_back(Id(tok.text));
    }
  }

  static void trace(Ctx &ctx, OpSeq &ops) {
    Exec &exe(ctx.coro.exec);
    
    for (auto &op: ops) {
      switch (op.code) {
      case OP_CALL:
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
	break;
      default:
	ERROR(Snabel, fmt("Invalid op-code: %0", op.code));
      }
    }
  }

  static void compile_expr(Compiler &cpr,
			   size_t lnr,
			   const std::vector<Tok> &exp,
			   OpSeq &out) {
    OpSeq ops;
    
    if (exp[0].text == "let") {
      if (exp.size() < 3) {
	ERROR(Snabel, fmt("Malformed let statement in line %0", lnr));
      } else {
	compile_expr(cpr, lnr,
		     std::vector<Tok>(std::next(exp.begin(), 2),
				      exp.end()),
		     ops);

	ops.emplace_back(Let(exp[1].text));
      }
    } else {
      for (auto t: exp) { compile_tok(cpr, lnr, t, ops); }	  
    }

    trace(cpr.ctx, ops);
    std::copy(ops.begin(), ops.end(),
	      std::back_inserter(out));
  }

  
  void compile(Compiler &cpr, const str &in) {
    size_t lnr(0);
    
    for (auto &ln: split_lines(in)) {
      if (!ln.empty()) {
	if (!cpr.ops.empty()) {
	  cpr.ops.emplace_back(Reset());
	}
	
	compile_expr(cpr, lnr, parse_expr(ln), cpr.ops);
      }
      
      lnr++;
    }
  }
  
  OpSeq compile(Ctx &ctx, const str &in) {
    Compiler cpr(ctx);
    compile(cpr, in);
    return cpr.ops;
  }
}

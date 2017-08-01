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

  static OpSeq compile_ln(Compiler &cpr, size_t lnr, const str &in) {
    auto toks(parse_expr(in));
    OpSeq out;
    
    if (toks[0].text == "let") {
      if (toks.size() < 3) {
	ERROR(Snabel, fmt("Missing let value at line %0:\n%1", lnr, in));
      } else {
	for (auto i(std::next(toks.begin(), 2)); i != toks.end(); i++) {
	  compile_tok(cpr, lnr, *i, out);
	}
	
	out.emplace_back(Let(toks[1].text));
      }
    } else {
      for (auto t: toks) { compile_tok(cpr, lnr, t, out); }	  
    }

    return out;
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
  
  void compile(Compiler &cpr, const str &in) {
    size_t lnr(0);
    
    for (auto &ln: split_lines(in)) {
      if (!ln.empty()) {
	auto ln_ops(compile_ln(cpr, lnr, ln));
	
	if (!cpr.ops.empty() && !ln_ops.empty()) {
	  cpr.ops.emplace_back(Reset());
	}

	trace(cpr.ctx, ln_ops);
	std::copy(ln_ops.begin(), ln_ops.end(),
		  std::back_inserter(cpr.ops));
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

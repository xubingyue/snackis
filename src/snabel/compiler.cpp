#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"

namespace snabel {  
  void compile(Exec &exe,
	       size_t lnr,
	       const Tok &tok,
	       OpSeq &out) {
    if (tok.text[0] == '(') {
      out.emplace_back(Op::make_begin());

      for (auto &e: parse_exprs(tok.text.substr(1, tok.text.size()-2))) {
	compile(exe, lnr, parse_expr(e), out);
      }
      
      out.emplace_back(Op::make_end());
    } else if (tok.text[0] == '@') {
      out.emplace_back(Op::make_label(tok.text.substr(1)));
    } else if (tok.text == "apply") {
      out.emplace_back(Op::make_apply());
    } else if (tok.text == "begin") {
      out.emplace_back(Op::make_begin());
    } else if (tok.text == "end") {
      out.emplace_back(Op::make_end());
    } else if (tok.text == "reset") {
      out.emplace_back(Op::make_reset());
    } else if (tok.text == "stash") {
      out.emplace_back(Op::make_stash());
    } else if (isdigit(tok.text[0]) || 
	(tok.text.size() > 1 && tok.text[0] == '-' && isdigit(tok.text[1]))) {
      out.emplace_back(Op::make_push(Box(exe.i64_type, to_int64(tok.text))));
    }  else {
      out.emplace_back(Op::make_id(tok.text));
    }
  }

  void compile(Exec &exe,
	       size_t lnr,
	       const TokSeq &exp,
	       OpSeq &out) {
    if (exp.empty()) { return; }
    
    if (exp[0].text == "let") {
      if (exp.size() < 3) {
	ERROR(Snabel, fmt("Malformed let statement in line %0", lnr));
      } else {
	compile(exe, lnr,
		TokSeq(std::next(exp.begin(), 2),
		       exp.end()),
		out);
	
	out.emplace_back(Op::make_let(exp[1].text));
      }
    } else {
      for (auto t: exp) { compile(exe, lnr, t, out); }	  
    }
  }
}

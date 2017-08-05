#include <iostream>
#include "snabel/compiler.hpp"
#include "snabel/error.hpp"
#include "snabel/exec.hpp"

namespace snabel {  
  void compile(Exec &exe,
	       size_t lnr,
	       const Tok &tok,
	       OpSeq &out) {
    if (tok.text[0] == '{') {      
      OpSeq seq;
      str e(tok.text.substr(1, tok.text.size()-2));
      compile(exe, lnr, parse_expr(e), seq);
      out.push_back(Op::make_push(Box(exe.op_seq_type,
				      seq)));
    } else if (tok.text[0] == '(') {
      out.push_back(Op::make_begin());
      str e(tok.text.substr(1, tok.text.size()-2));
      compile(exe, lnr, parse_expr(e), out);
      out.push_back(Op::make_end());
    } else if (tok.text[0] == '@') {
      out.push_back(Op::make_label(tok.text.substr(1)));
    } else if (tok.text[0] == '!') {
      out.push_back(Op::make_jump(tok.text.substr(1)));
    } else if (tok.text[0] == '"') {
      out.push_back(Op::make_push(Box(exe.str_type,
				      tok.text.substr(1, tok.text.size()-2))));
    } else if (tok.text == "begin") {
      out.push_back(Op::make_begin());
    } else if (tok.text == "drop") {
      out.push_back(Op::make_drop());
    } else if (tok.text == "end") {
      out.push_back(Op::make_end());
    } else if (tok.text == "reset") {
      out.push_back(Op::make_reset());
    } else if (isdigit(tok.text[0]) || 
	(tok.text.size() > 1 && tok.text[0] == '-' && isdigit(tok.text[1]))) {
      out.push_back(Op::make_push(Box(exe.i64_type, to_int64(tok.text))));
    }  else {
      out.push_back(Op::make_id(tok.text));
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
	auto i(std::next(exp.begin(), 2));
	
	for (; i != exp.end(); i++) {
	  if (i->text == ";") {
	    i++;
	    break;
	  }
	  
	  compile(exe, lnr, *i, out);
	}
	
	out.push_back(Op::make_let(exp[1].text));

	if (i != exp.end()) {
	  compile(exe, lnr, TokSeq(i, exp.end()), out);
	}
      }
    } else {
      for (auto t: exp) { compile(exe, lnr, t, out); }	  
    }
  }
}

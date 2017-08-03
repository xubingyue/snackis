#ifndef SNABEL_COMPILER_HPP
#define SNABEL_COMPILER_HPP

#include "snabel/op.hpp"
#include "snabel/parser.hpp"

namespace snabel {
  struct Compiler {
    Scope &scope;
    OpSeq ops;
    
    Compiler(Scope &scp);
  };

  void compile(Compiler &cpr,
	       size_t lnr,
	       const Tok &tok,
	       OpSeq &out);

  void compile(Compiler &cpr,
	       size_t lnr,
	       const TokSeq &exp,
	       OpSeq &out);
  
  void compile(Compiler &cpr, const str &in);
}

#endif

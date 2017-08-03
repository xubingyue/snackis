#ifndef SNABEL_COMPILER_HPP
#define SNABEL_COMPILER_HPP

#include "snabel/op.hpp"
#include "snabel/parser.hpp"

namespace snabel {
  struct Exec;
  
  void compile(Exec &exe,
	       size_t lnr,
	       const Tok &tok,
	       OpSeq &out);

  void compile(Exec &exe,
	       size_t lnr,
	       const TokSeq &exp,
	       OpSeq &out);
}

#endif

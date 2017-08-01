#ifndef SNABEL_COMPILER_HPP
#define SNABEL_COMPILER_HPP

#include "snabel/op.hpp"

namespace snabel {
  struct Compiler {
    Ctx &ctx;
    OpSeq ops;
    
    Compiler(Ctx &ctx);
  };

  void compile(Compiler &cpr, const str &in);
  OpSeq compile(Ctx &ctx, const str &in);
}

#endif

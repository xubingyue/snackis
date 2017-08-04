#ifndef SNABEL_PARSER_HPP
#define SNABEL_PARSER_HPP

#include <deque>
#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Tok {
    str text;
    size_t i;
    
    Tok(const str &txt, size_t i=-1);
  };

  using StrSeq = std::deque<str>;
  using TokSeq = std::deque<Tok>;

  StrSeq parse_lines(const str &in);
  size_t parse_parens(const str &in);
  TokSeq parse_expr(const str &in);
}

#endif

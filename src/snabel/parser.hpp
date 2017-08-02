#ifndef SNABEL_PARSER_HPP
#define SNABEL_PARSER_HPP

#include "snackis/core/str.hpp"

namespace snabel {
  using namespace snackis;
  
  struct Expr {
    str text;
    size_t i, j;
    
    Expr(const str &txt, size_t i=-1);
  };

  struct Tok {
    str text;
    size_t i;
    
    Tok(const str &txt, size_t i=-1);
  };

  std::vector<str> parse_lines(const str &in);
  std::vector<Expr> parse_exprs(const str &in);
  size_t parse_parens(const str &in);
  std::vector<Tok> parse_expr(const Expr &in);
}

#endif

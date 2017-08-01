#include <iostream>

#include "snabel/error.hpp"
#include "snabel/parser.hpp"

namespace snabel {
  Expr::Expr(const str &txt, size_t i):
    text(txt),
    i(i)
  { }

  Tok::Tok(const str &txt, size_t i):
    text(txt),
    i(i)
  { }

  std::vector<str> parse_lines(const str &in) {
    size_t i(0), j(0);
    std::vector<str> out;
    
    while ((j=in.find('\n', j)) != str::npos) {
      if (j > 0 && in[j-1] == '\\') {
	j += 2;
      } else if (j > i) {
	out.push_back(trim(in.substr(i, j-i)));
	j++;
	i = j;
      } else {
	j++;
	i = j;
      }
    }

    if (i < in.size()) { out.push_back(trim(in.substr(i))); }
    return out;
  }

  std::vector<Expr> parse_line(const str &in) {
    size_t i(0);
    bool quoted(false);
    std::vector<Expr> out;
    
    for (size_t j(0); j < in.size(); j++) {
      auto &c(in[j]);
      switch(c) {
      case '"':
	if (j == 0 || in[j-1] != '\\') { quoted = !quoted; }
	break;
      case ';':
	if (j > i && !quoted) {
	  out.emplace_back(trim(in.substr(i, j-i)), i);
	  i = j+1;
	  continue;
	}
	
	break;
      }

      if (j == in.size()-1) {
	out.emplace_back(trim(in.substr(i, j-i+1)), i);
      }
    }

    return out;
  }

  size_t parse_parens(const str &in) {
    bool quoted(false);
    int depth(1);
    
    for (size_t i(1); i < in.size(); i++) {
      auto &c(in[i]);
      switch(c) {
      case '"':
	if (i == 0 || in[i-1] != '\\') { quoted = !quoted; }
	break;
      case '(':
	depth++;
	break;
      case ')':
	depth--;
	if (!depth) { return i; }
	break;
      }
    }

    return str::npos;
  }
  
  std::vector<Tok> parse_expr(const Expr &in) {
    size_t i(0);
    bool quoted(false);
    std::vector<Tok> out;
    
    for (size_t j(0); j < in.text.size(); j++) {
      auto c(in.text[j]);

      switch(c) {	
      case '"':
	if (j == 0 || in.text[j-1] != '\\') { quoted = !quoted; }
	break;
      case '\\':
      case '(':
      case '\n':
      case ' ':
	if (j > i && !quoted) {
	  out.emplace_back(trim(in.text.substr(i, j-i)), in.i+i);
	  
	  while (j < in.text.size()-2 && (c == '\\' || isspace(c))) {
	    j++;
	    c = in.text[j];
	  }
	  
	  i = j;
	}
	
	break;
      }

      if (j == in.text.size()-1) {
	out.emplace_back(trim(in.text.substr(i)), in.i+i);
      } else if (c == '(') {
	size_t k(j);
	str rest(in.text.substr(k));
	k = parse_parens(rest);

	if (k == str::npos) {
	  ERROR(Snabel, "Unbalanced parenthesis");
	  return out;
	}

	out.emplace_back(rest.substr(0, k+1), in.i+j);
	j += k+1;
	i = j;
      }
    }

    return out;
  }  
}

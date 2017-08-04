#include <iostream>

#include "snabel/error.hpp"
#include "snabel/parser.hpp"

namespace snabel {
  Tok::Tok(const str &txt, size_t i):
    text(txt),
    i(i)
  { }

  StrSeq parse_lines(const str &in) {
    size_t i(0), j(0);
    StrSeq out;
    
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

  size_t parse_pair(const str &in, char fst, char lst) {
    bool quoted(false);
    int depth(1);
    
    for (size_t i(1); i < in.size(); i++) {
      auto &c(in[i]);

      if (c == '"') {
	if (i == 0 || in[i-1] != '\\') { quoted = !quoted; }
      } else if (c == fst) {
	depth++;
      } else if (c == lst) {
	depth--;
	if (!depth) { return i; }
      }
    }

    return str::npos;
  }
  
  TokSeq parse_expr(const str &in) {
    size_t i(0);
    bool quoted(false);
    TokSeq out;
    
    for (size_t j(0); j < in.size(); j++) {
      auto c(in[j]);

      switch(c) {	
      case '"':
	if (j == 0 || in[j-1] != '\\') { quoted = !quoted; }
	if (quoted) { break; }
	j++;
      case '\\':
      case '{':
      case '(':
      case '\n':
      case ';':
      case ' ':
	if (j > i) {
	  str s(trim(in.substr(i, j-i)));
	  if (!s.empty()) { out.emplace_back(s, i); }
	  
	  while (j < in.size()-2 && (c == '\\' || isspace(c))) {
	    j++;
	    c = in[j];
	  }
	  
	  i = j;
	}
	
	break;
      }

      if (j == in.size()-1) {
	str s(trim(in.substr(i)));
	if (!s.empty()) { out.emplace_back(s, i); }
      } else if (c == '{' || c == '(') {
	size_t k(j);
	str rest(in.substr(k));
	k = parse_pair(rest, c, (c == '(') ? ')' : '}');

	if (k == str::npos) {
	  ERROR(Snabel, fmt("Unbalanced %0", c));
	  return out;
	}

	out.emplace_back(rest.substr(0, k+1), j);
	j += k+1;
	i = j;
      } else if (c == ';') {
	out.emplace_back(";", j);
	i++;
      }
    }

    return out;
  }  
}

#ifndef SNACKIS_UI_HEADER_HPP
#define SNACKIS_UI_HEADER_HPP

#include <stack>
#include "snackis/core/str.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  struct Header: public Window {
    std::stack<str> label;
    
    Header(Ctx &ctx);
  };

  void push_label(Header &hdr, const str &lbl);
  void pop_label(Header &hdr);
}}

#endif

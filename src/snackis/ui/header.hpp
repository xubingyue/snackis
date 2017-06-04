#ifndef SNACKIS_UI_HEADER_HPP
#define SNACKIS_UI_HEADER_HPP

#include "snackis/core/str.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  struct Header: public Window {
    str label;
    Header(Ctx &ctx);
  };

  void set_label(Header &hdr, const str &lbl);
}}

#endif

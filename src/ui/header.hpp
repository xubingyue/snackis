#ifndef SNACKIS_UI_HEADER_HPP
#define SNACKIS_UI_HEADER_HPP

#include "snackis/core/str.hpp"
#include "ui/window.hpp"

namespace ui {
  struct Header: public Window {
    str label;
    Header(Ctx &ctx);
  };

  void set_label(Header &hdr, const str &lbl);
}

#endif

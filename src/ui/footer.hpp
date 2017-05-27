#ifndef SNACKIS_UI_FOOTER_HPP
#define SNACKIS_UI_FOOTER_HPP

#include "snackis/core/str.hpp"
#include "ui/window.hpp"

namespace ui {
  struct Footer: public Window {
    Footer(Ctx &ctx);
  };

  void set_status(Footer &hdr, const str &stat);
}

#endif

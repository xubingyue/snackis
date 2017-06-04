#ifndef SNACKIS_UI_FOOTER_HPP
#define SNACKIS_UI_FOOTER_HPP

#include "snackis/core/str.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  struct Footer: public Window {
    str status;
    Footer(Ctx &ctx);
  };

  void set_status(Footer &hdr, const str &stat);
}}

#endif

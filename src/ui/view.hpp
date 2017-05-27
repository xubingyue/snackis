#ifndef SNACKIS_UI_VIEW_HPP
#define SNACKIS_UI_VIEW_HPP

#include "ui/window.hpp"

namespace ui {
  struct Header;
  struct Footer;
  
  struct View: public Window {
    Header &header;
    Footer &footer;

    View(Ctx &ctx, Header &hdr, Footer &ftr);
  };
}

#endif

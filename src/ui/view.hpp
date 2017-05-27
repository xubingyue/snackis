#ifndef SNACKIS_UI_VIEW_HPP
#define SNACKIS_UI_VIEW_HPP

#include "ui/window.hpp"

namespace ui {
  struct Header;
  
  struct View: public Window {
    Header &header;

    View(Ctx &ctx, Header &hdr);
  };
}

#endif

#ifndef SNACKIS_UI_VIEW_HPP
#define SNACKIS_UI_VIEW_HPP

#include "ui/window.hpp"

namespace ui {
  struct View: public Window {
    View(Ctx &ctx);
  };
}

#endif

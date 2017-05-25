#ifndef SNACKIS_UI_CONSOLE_HPP
#define SNACKIS_UI_CONSOLE_HPP

#include "ui/window.hpp"

namespace ui {
  struct Console: public Window {
    Console(Ctx &ctx);
  };

  void log(Console &cons, const str &msg);
}

#endif

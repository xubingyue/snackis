#ifndef SNACKIS_UI_WINDOW_HPP
#define SNACKIS_UI_WINDOW_HPP

#include <ncurses.h>
#include "snackis/core/str.hpp"
#include "ui/ui.hpp"

namespace snackis {
  struct Ctx;
}

namespace ui {
  using namespace snackis;
  
  struct Window {
    Ctx &ctx;
    Dim dim;
    Pos pos;
    WINDOW *ptr;
    
    Window(Ctx &ctx, const Dim &dim, const Pos &pos);
    virtual ~Window();
  };

  void print(Window &wnd, const str &it);
  void refresh(Window &wnd);
  int get_key(Window &wnd);
}

#endif

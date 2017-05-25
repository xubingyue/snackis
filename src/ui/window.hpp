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

  void set_bg(Window &wnd, chtype ch);
  void move(Window &wnd, const Pos &pos);
  void print(Window &wnd, const str &msg);
  void refresh(Window &wnd);
  int get_key(Window &wnd);
}

#endif

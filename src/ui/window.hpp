#ifndef SNACKIS_UI_WINDOW_HPP
#define SNACKIS_UI_WINDOW_HPP

#include <ncurses.h>
#include "snackis/core/str.hpp"
#include "ui/ui.hpp"

namespace ui {
  using namespace snackis;
  
  struct Window {
    Dim dim;
    Pos pos;
    WINDOW *ptr;
    
    Window(const Dim &dim, const Pos &pos);
    virtual ~Window();
  };

  void print(Window &wnd, const str &it);
  void refresh(Window &wnd);
  int get_key(Window &wnd);
}

#endif

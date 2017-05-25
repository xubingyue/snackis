#include "ui/window.hpp"

namespace ui {
  Window::Window(Ctx &ctx, const Dim &dim, const Pos &pos):
    ctx(ctx), dim(dim), pos(pos), ptr(newwin(dim.h, dim.w, pos.y, pos.x)) {
    keypad(ptr, true);
  }

  Window::~Window() { delwin(ptr); }

  void set_bg(Window &wnd, chtype ch) { wbkgd(wnd.ptr, ch); }

  void move(Window &wnd, const Pos &pos) { wmove(wnd.ptr, pos.y, pos.x); }
  
  void print(Window &wnd, const str &msg) { waddstr(wnd.ptr, msg.c_str()); }

  void refresh(Window &wnd) { wrefresh(wnd.ptr); }

  int get_key(Window &wnd) { return wgetch(wnd.ptr); }
}


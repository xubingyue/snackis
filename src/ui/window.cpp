#include "ui/window.hpp"

namespace ui {
  Window::Window(const Dim &dim, const Pos &pos):
    dim(dim), pos(pos), ptr(newwin(dim.h, dim.w, pos.y, pos.x))
  { }

  Window::~Window() { delwin(ptr); }

  void print(Window &wnd, const str &it) { waddstr(wnd.ptr, it.c_str()); }

  void refresh(Window &wnd) { wrefresh(wnd.ptr); }

  int get_key(Window &wnd) { return wgetch(wnd.ptr); }
}


#include <ncurses.h>
#include "snackis/ui/ui.hpp"

namespace snackis {
namespace ui {
  Dim::Dim(int h, int w): h(h), w(w) { }

  Pos::Pos(int y, int x): y(y), x(x) { }

  void open() {
    initscr();
    raw();
    noecho();
  }

  void close() { endwin(); }

  Dim dim() {
    int y, x;
    getmaxyx(stdscr, y, x);
    return Dim(y, x);
  }
}}

#include "ui/console.hpp"

namespace ui {
  Console::Console(): Window(ui::Dim(ui::dim().h-1, ui::dim().w/2), ui::Pos(0, 0))
  { }

  void log(Console &cons, const str &msg) {
    print(cons, msg + "\n");
    refresh(cons);
  }
}

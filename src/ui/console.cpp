#include "snackis/core/format.hpp"
#include "snackis/core/time.hpp"
#include "ui/console.hpp"

namespace ui {
  Console::Console(Ctx &ctx):
    Window(ctx, ui::Dim(ui::dim().h-2, ui::dim().w/2), ui::Pos(1, 0)) {
    move(*this, Pos(1, 0));
  }

  void log(Console &cons, const str &msg) {
    print(cons, format("{0} {1}\n", format_time(now(), "%a %H:%M:%S"), msg));
    refresh(cons);
  }
}

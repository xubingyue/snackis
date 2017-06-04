#include "snackis/core/fmt.hpp"
#include "snackis/core/time.hpp"
#include "snackis/ui/console.hpp"

namespace snackis {
namespace ui {
  Console::Console(Ctx &ctx):
    Window(ctx, ui::Dim(ui::dim().h-2, ui::dim().w/2), ui::Pos(1, 0)) {
    move(*this, Pos(1, 0));
  }

  void log(Console &cons, const str &msg) {
    print(cons, fmt("%0 %1\n", fmt(now(), "%a %H:%M:%S"), msg));
    refresh(cons);
  }
}}

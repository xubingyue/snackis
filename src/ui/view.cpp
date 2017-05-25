#include "ui/view.hpp"

namespace ui {
  View::View(Ctx &ctx):
    Window(ctx, ui::Dim(ui::dim().h-2, ui::dim().w/2), ui::Pos(1, ui::dim().w/2))
  { }
}

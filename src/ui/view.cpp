#include "ui/header.hpp"
#include "ui/view.hpp"

namespace ui {
  View::View(Ctx &ctx, Header &header):
    Window(ctx, ui::Dim(ui::dim().h-2, ui::dim().w/2), ui::Pos(1, ui::dim().w/2)),
    header(header)
  { }

  void set_label(View &view, const str &lbl) {
    set_label(view.header, lbl);
  }
}

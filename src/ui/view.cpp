#include "ui/header.hpp"
#include "ui/footer.hpp"
#include "ui/view.hpp"

namespace ui {
  View::View(Ctx &ctx, Header &hdr):
    Window(ctx, ui::Dim(ui::dim().h-2, ui::dim().w/2), ui::Pos(1, ui::dim().w/2)),
    header(hdr)
  { }
}

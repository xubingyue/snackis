#include "snackis/ui/header.hpp"
#include "snackis/ui/footer.hpp"
#include "snackis/ui/view.hpp"

namespace snackis {
namespace ui {
  View::View(Ctx &ctx, Header &hdr):
    Window(ctx, ui::Dim(ui::dim().h-2, ui::dim().w/2), ui::Pos(1, ui::dim().w/2)),
    header(hdr)
  { }
}}

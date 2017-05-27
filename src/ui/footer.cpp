#include "snackis/snackis.hpp"
#include "snackis/core/fmt.hpp"

#include "ui/footer.hpp"

namespace ui {
  Footer::Footer(Ctx &ctx):
    Window(ctx, ui::Dim(1, ui::dim().w/2), ui::Pos(ui::dim().h-1, ui::dim().w/2)) {
    set_bg(*this, A_REVERSE);
    refresh(*this);
  }

  void set_status(Footer &ftr, const str &stat) {
    move(ftr, Pos(0, 0));
    wclrtoeol(ftr.ptr);
    print(ftr, stat);
    refresh(ftr);
  }
}

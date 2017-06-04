#include "snackis/snackis.hpp"
#include "snackis/ui/footer.hpp"

namespace snackis {
namespace ui {
  Footer::Footer(Ctx &ctx):
    Window(ctx, ui::Dim(1, ui::dim().w/2), ui::Pos(ui::dim().h-1, ui::dim().w/2)) {
    set_bg(*this, A_REVERSE);
    print(*this, "?");
    refresh(*this);
  }

  void set_status(Footer &ftr, const str &stat) {
    move(ftr, Pos(0, 2));
    wclrtoeol(ftr.ptr);
    print(ftr, stat);
    ftr.status = stat;
    refresh(ftr);
  }
}}

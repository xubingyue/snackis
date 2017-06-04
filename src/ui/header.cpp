#include "snackis/snackis.hpp"
#include "snackis/core/format.hpp"

#include "ui/header.hpp"

namespace ui {
  Header::Header(Ctx &ctx): Window(ctx, ui::Dim(1, ui::dim().w), ui::Pos(0, 0)) {
    set_bg(*this, A_REVERSE);
    print(*this, format("Snackis v{0}", version_str()));
    refresh(*this);
  }

  void set_label(Header &hdr, const str &lbl) {
    move(hdr, Pos(0, hdr.dim.w/2));
    wclrtoeol(hdr.ptr);
    move(hdr, Pos(0, hdr.dim.w/2));
    print(hdr, lbl);
    hdr.label = lbl;
    refresh(hdr);
  }
}

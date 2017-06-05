#include "snackis/snackis.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/ui/header.hpp"

namespace snackis {
namespace ui {
  Header::Header(Ctx &ctx): Window(ctx, ui::Dim(1, ui::dim().w), ui::Pos(0, 0)) {
    set_bg(*this, A_REVERSE);
    print(*this, fmt("Snackis v%0", version_str()));
    refresh(*this);
  }

  static void print_label(Header &hdr, const str &lbl) {
    move(hdr, Pos(0, hdr.dim.w/2));
    wclrtoeol(hdr.ptr);
    move(hdr, Pos(0, hdr.dim.w/2));
    print(hdr, lbl);
    refresh(hdr);
  }
  
  void push_label(Header &hdr, const str &lbl) {
    hdr.label.push(lbl);
    print_label(hdr, lbl);
  }

  void pop_label(Header &hdr) {
    hdr.label.pop();
    print_label(hdr, hdr.label.empty() ? "" : hdr.label.top());
  }
}}

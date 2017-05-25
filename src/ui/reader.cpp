#include "ui/reader.hpp"

namespace ui {
  Reader::Reader(): Window(ui::Dim(1, ui::dim().w), ui::Pos(ui::dim().h-1, 0)) {
    print(*this, ": ");
    refresh(*this);
  }
}

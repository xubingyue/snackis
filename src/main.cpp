#include "ui/ui.hpp"
#include "ui/window.hpp"

using namespace snackis;

int main() {
  ui::open();
  ui::Dim max(ui::dim());
  ui::Window console(ui::Dim(max.h-1, max.w/2), ui::Pos(0, 0));
  ui::print(console, "Welcome to Snackis!\n");
  ui::refresh(console);
  ui::get_key(console);
  ui::close();
  return 0;
}

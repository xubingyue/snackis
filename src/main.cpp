#include "ui/ui.hpp"
#include "ui/console.hpp"

using namespace snackis;

int main() {
  ui::open();
  ui::Dim max(ui::dim());
  ui::Console console;
  ui::log(console, "Welcome to Snackis!");
  ui::get_key(console);
  ui::close();
  return 0;
}

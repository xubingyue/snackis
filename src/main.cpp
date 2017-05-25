#include "ui/ui.hpp"
#include "ui/console.hpp"
#include "ui/reader.hpp"

using namespace snackis;

int main() {
  ui::open();
  ui::Dim max(ui::dim());
  ui::Console console;
  ui::log(console, "Welcome to Snackis!");
  ui::Reader reader;
  ui::get_key(reader);
  ui::close();
  return 0;
}

#include "snackis/ctx.hpp"
#include "ui/ui.hpp"
#include "ui/console.hpp"
#include "ui/reader.hpp"

using namespace snackis;

int main() {
  db::Ctx db_ctx("db/");
  Ctx ctx(db_ctx);

  ui::open();
  ui::Dim max(ui::dim());

  ui::Console console(ctx);
  ctx.logger = [&console](const str &msg) { ui::log(console, msg); };
  log(ctx, "Welcome to Snackis!");

  ui::Reader reader(ctx);
  ui::run(reader);
  ui::close();
  return 0;
}

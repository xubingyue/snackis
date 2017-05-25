#include "snackis/ctx.hpp"
#include "ui/ui.hpp"
#include "ui/console.hpp"
#include "ui/header.hpp"
#include "ui/reader.hpp"

using namespace snackis;

int main() {
  db::Ctx db_ctx("db/");
  Ctx ctx(db_ctx);

  ui::open();
  ui::Dim max(ui::dim());

  ui::Header hdr(ctx);
  ui::set_label(hdr, "Login");
  
  ui::Console cons(ctx);
  ctx.log = [&cons](const str &msg) { ui::log(cons, msg); };
  log(ctx, "Welcome to Snackis!");

  ui::Reader rdr(ctx);
  ui::run(rdr);
  ui::close();
  return 0;
}

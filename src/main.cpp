#include "snackis/ctx.hpp"
#include "ui/console.hpp"
#include "ui/footer.hpp"
#include "ui/header.hpp"
#include "ui/login_form.hpp"
#include "ui/reader.hpp"
#include "ui/view.hpp"
#include "ui/ui.hpp"

using namespace snackis;

static bool login(ui::View &view) {
  ui::LoginForm login(view);
  ui::open(login);
  return ui::run(login);
}

static void run(Ctx &ctx) {
  ui::Header hdr(ctx);
  ui::Footer ftr(ctx);
  
  ui::Console cons(ctx);
  ctx.log = [&cons](const str &msg) { ui::log(cons, msg); };
  log(ctx, "Welcome to Snackis");
  ui::View view(ctx, hdr, ftr);

  if (login(view)) {
    open(ctx);
    
    Peer &me(whoami(ctx));
    if (!me.name.empty()) { log(ctx, fmt("Hello %1%") % me.name); }
    
    log(ctx, "Ctrl-q cancels current operation");
    log(ctx, "Type 'quit' followed by Return to exit");
    ui::Reader rdr(ctx, view);
    ui::run(rdr);
  }
}

int main() {
  ui::open();
  Ctx ctx("db/");
  run(ctx);
  ui::close();
  return 0;
}

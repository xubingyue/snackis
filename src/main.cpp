#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/ui/console.hpp"
#include "snackis/ui/footer.hpp"
#include "snackis/ui/header.hpp"
#include "snackis/ui/login_form.hpp"
#include "snackis/ui/reader.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/ui.hpp"

using namespace snackis;

static bool login(ui::View &view, ui::Footer &ftr) {
  ui::LoginForm login(view, ftr);
  ui::open(login);
  return ui::run(login);
}

static void run(Ctx &ctx) {
  ui::Header hdr(ctx);
  ui::Footer ftr(ctx);
  
  ui::Console cons(ctx);
  ctx.log = [&cons](const str &msg) { ui::log(cons, msg); };
  log(ctx, "Welcome to Snackis");
  ui::View view(ctx, hdr);

  if (login(view, ftr)) {
    open(ctx);
    
    Peer &me(whoami(ctx));
    if (!me.name.empty()) { log(ctx, fmt("Welcome back, %0", me.name)); }
    
    ui::Reader rdr(ctx, cons, view, ftr);
    ui::run(rdr);
  }
}

int main() {
  ui::open();
  snackis::Ctx ctx("db/");
  run(ctx);
  ui::close();
  return 0;
}

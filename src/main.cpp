#include "snackis/ctx.hpp"
#include "ui/console.hpp"
#include "ui/header.hpp"
#include "ui/login_form.hpp"
#include "ui/reader.hpp"
#include "ui/view.hpp"
#include "ui/ui.hpp"

using namespace snackis;

int main() {
  Ctx ctx("db/");

  ui::open();
  ui::Dim max(ui::dim());

  {
    ui::Header hdr(ctx);
    ui::Console cons(ctx);
    ctx.log = [&cons](const str &msg) { ui::log(cons, msg); };
    log(ctx, "Welcome to Snackis");
    ui::View view(ctx, hdr);
  
    {  
      ui::LoginForm login(view);
      ui::open(login);
      ui::run(login);
    }

    open(ctx);
    slurp(ctx);
    log(ctx, "Type 'quit' followed by Return to exit");
    ui::Reader rdr(ctx);
    ui::run(rdr);
  }
  
  ui::close();
  return 0;
}

#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "ui/reader.hpp"

namespace ui {
  Reader::Reader(Ctx &ctx):
    Window(ctx, ui::Dim(1, ui::dim().w), ui::Pos(ui::dim().h-1, 0)),
    form(*this),
    field(form, ui::Dim(1, dim.w), ": "),
    last_cmd(none),
    quitting(false) {
    open(form);
    refresh(*this);
    init_cmds(*this);
  }

  void init_cmds(Reader &rdr) {
    rdr.cmds["quit"] = [&rdr]() { rdr.quitting = true; };
  }

  bool run_cmd(Reader &rdr, const str &in) {
    if (in == "") {
      if (rdr.last_cmd) {
	rdr.last_cmd.get()();
      }
    } else {
      const str id(in.substr(0, in.find_first_of(' ')));
      auto found = rdr.cmds.find(id);
      if (found == rdr.cmds.end()) { return false; }
      rdr.last_cmd = found->second;
      found->second();
    }

    return true;
  }
  
  void run_once(Reader &rdr) {
    bool done = false;

    while (!done) {
      int key = get_key(rdr);
      if (key == KEY_RETURN) { done = true; }
      drive(rdr.form, key);
    }

    str in(trim(get_str(rdr.field)));
    clear_field(rdr.form);
    refresh(rdr);
    if (!run_cmd(rdr, in)) { 
      log(rdr.ctx, fmt("Invalid command: '%1%'") % in); 
    }
  }
  
  void run(Reader &rdr) {
    while (!rdr.quitting) {
      run_once(rdr);
      focus(rdr.form);
    }
  }
}

#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"
#include "snackis/ui/invite_form.hpp"
#include "snackis/ui/reader.hpp"
#include "snackis/ui/profile_form.hpp"

namespace snackis {
namespace ui {
  Reader::Reader(Ctx &ctx, View &view, Footer &ftr):
    Window(ctx, ui::Dim(1, ui::dim().w/2), ui::Pos(ui::dim().h-1, 0)),
    form(*this, ftr),
    field(form, ui::Dim(1, dim.w), "! "),
    last_cmd(nullopt),
    quitting(false),
    view(view) {
    form.status = "Type 'quit' followed by Return to exit";
    open(form);
    refresh(*this);
    init_cmds(*this);
  }

  void init_cmds(Reader &rdr) {
    rdr.cmds["profile"] = [&rdr]() {
      ProfileForm prof(rdr.view, rdr.form.footer);
      open(prof);
      run(prof);
    };

    rdr.cmds["invite"] = [&rdr]() {
      InviteForm inv(rdr.view, rdr.form.footer);
      open(inv);
      run(inv);
    };

    rdr.cmds["fetch"] = [&rdr]() { 
      Imap imap(rdr.ctx);
      fetch(imap); 
    };

    rdr.cmds["send"] = [&rdr]() { 
      Smtp smtp(rdr.ctx);
      send(smtp); 
    };

    rdr.cmds["quit"] = [&rdr]() { rdr.quitting = true; };
  }

  bool run_cmd(Reader &rdr, const str &in) {
    if (in == "") {
      if (rdr.last_cmd) {
	(*rdr.last_cmd)();
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
      chtype ch = get_key(rdr);
      if (ch == KEY_RETURN) { done = true; }
      drive(rdr.form, ch);
    }
   
    str in(trim(get_str(rdr.field)));
    clear_field(rdr.form);
    refresh(rdr);
    
    try {
      if (!run_cmd(rdr, in)) { 
	log(rdr.ctx, fmt("Invalid command: '%0'", in));
      }
    } catch (const std::exception &e) {
      log(rdr.ctx, e.what());
    }
  }
  
  void run(Reader &rdr) {
    while (!rdr.quitting) {
      run_once(rdr);
      focus(rdr.form);
    }
  }
}}

#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"
#include "snackis/ui/invite_form.hpp"
#include "snackis/ui/reader.hpp"
#include "snackis/ui/profile_form.hpp"

namespace snackis {
namespace ui {
  static void init_cmds(Reader &rdr) {
    push(rdr.field, "profile", Reader::Cmd([&rdr]() {
	  ProfileForm prof(rdr.view, rdr.form.footer);
	  open(prof);
	  run(prof);
	}));

    push(rdr.field, "invite", Reader::Cmd([&rdr]() {
	  InviteForm inv(rdr.view, rdr.form.footer);
	  open(inv);
	  run(inv);
	}));

    push(rdr.field, "fetch", Reader::Cmd([&rdr]() { 
	  Imap imap(rdr.ctx);
	  fetch(imap); 
	}));

    push(rdr.field, "send", Reader::Cmd([&rdr]() { 
	  Smtp smtp(rdr.ctx);
	  send(smtp); 
	}));

    push(rdr.field, "quit", Reader::Cmd([&rdr]() { rdr.quitting = true; }));
  }

  Reader::Reader(Ctx &ctx, View &view, Footer &ftr):
    Window(ctx, ui::Dim(1, ui::dim().w/2), ui::Pos(ui::dim().h-1, 0)),
    form(*this, ftr),
    field(form, ui::Dim(1, dim.w), "!"),
    last_cmd(nullopt),
    quitting(false),
    view(view) {
    form.status = "Type 'quit' followed by Return to exit";
    field.allow_clear = true;
    open(form);
    refresh(*this);
    init_cmds(*this);
  }

  static void run_once(Reader &rdr) {
    bool done = false;

    while (!done) {
      chtype ch = get_key(rdr);
      if (ch == KEY_RETURN) { done = true; }
      drive(rdr.form, ch);
    }
   
    clear_field(rdr.form);
    refresh(rdr);
    
    try {
      if (rdr.field.selected) {
	rdr.field.selected->val();
      } else {
	if (rdr.last_cmd) { (*rdr.last_cmd)(); }
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

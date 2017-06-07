#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"
#include "snackis/ui/decrypt_form.hpp"
#include "snackis/ui/encrypt_form.hpp"
#include "snackis/ui/inbox_form.hpp"
#include "snackis/ui/invite_form.hpp"
#include "snackis/ui/reader.hpp"
#include "snackis/ui/settings_form.hpp"

namespace snackis {
namespace ui {
  static void init_cmds(Reader &rdr) {
    insert(rdr.field, "settings", Reader::Cmd([&rdr]() {
	  SettingsForm frm(rdr.view, rdr.form.footer);
	  open(frm);
	  run(frm);
	}));

    insert(rdr.field, "invite", Reader::Cmd([&rdr]() {
	  InviteForm frm(rdr.view, rdr.form.footer);
	  open(frm);
	  run(frm);
	}));

    insert(rdr.field, "fetch", Reader::Cmd([&rdr]() { 
	  Imap imap(rdr.ctx);
	  fetch(imap); 
	}));

    insert(rdr.field, "send", Reader::Cmd([&rdr]() { 
	  if (rdr.ctx.db.outbox.recs.empty()) {
	    log(rdr.ctx, "Nothing to send");
	  } else {
	    Smtp smtp(rdr.ctx);
	    send(smtp);
	  }
	}));

    insert(rdr.field, "inbox", Reader::Cmd([&rdr]() { 
	  if (rdr.ctx.db.inbox.recs.empty()) {
	    log(rdr.ctx, "Inbox is empty");
	  } else {
	    InboxForm frm(rdr.view, rdr.form.footer);
	    open(frm);
	    run(frm);
	  }
	}));
    
    insert(rdr.field, "encrypt", Reader::Cmd([&rdr]() {
	  EncryptForm frm(rdr.view, rdr.form.footer);
	  open(frm);
	  run(frm);
	}));

    insert(rdr.field, "decrypt", Reader::Cmd([&rdr]() {
	  DecryptForm frm(rdr.view, rdr.form.footer);
	  open(frm);
	  run(frm);
	}));

    insert(rdr.field, "quit", Reader::Cmd([&rdr]() { rdr.quitting = true; }));
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

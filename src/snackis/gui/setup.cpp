#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/setup.hpp"
#include "snackis/net/imap.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
namespace gui {
  Server::Server(Ctx &ctx, ServerSettings &sts, GCallback fn):
    ctx(ctx),
    settings(sts),
    box(new_grid()),
    url(gtk_entry_new()),
    port(gtk_entry_new()),
    user(gtk_entry_new()),
    pass(gtk_entry_new()),
    poll(gtk_entry_new())
  {
    gtk_widget_set_margin_top(box, 5);

    int row(0);
    gtk_grid_attach(GTK_GRID(box), new_label("URL"), 0, row, 2, 1);
    gtk_widget_set_hexpand(url, true);
    gtk_grid_attach(GTK_GRID(box), url, 0, row+1, 2, 1);

    gtk_grid_attach(GTK_GRID(box), new_label("SSL Port"), 2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(box), port, 2, row+1, 1, 1);

    row += 2;
    gtk_grid_attach(GTK_GRID(box), new_label("User"), 0, row, 1, 1);
    gtk_widget_set_hexpand(user, true);
    gtk_grid_attach(GTK_GRID(box), user, 0, row+1, 1, 1);

    gtk_grid_attach(GTK_GRID(box), new_label("Password"), 1, row, 1, 1);
    gtk_entry_set_visibility(GTK_ENTRY(pass), false);
    gtk_widget_set_hexpand(pass, true);
    gtk_grid_attach(GTK_GRID(box), pass, 1, row+1, 1, 1);

    gtk_grid_attach(GTK_GRID(box), new_label("Poll (s)"), 2, row, 1, 1);
    gtk_grid_attach(GTK_GRID(box), poll, 2, row+1, 1, 1);

    row += 2;
    GtkWidget *btn = gtk_button_new_with_mnemonic("_Test Connection");
    gtk_widget_set_margin_top(btn, 5);
    g_signal_connect(btn, "clicked", G_CALLBACK(fn), this);
    gtk_grid_attach(GTK_GRID(box), btn, 2, row, 1, 1);
  }

  static void on_folder(GtkEntry *e, const str &lbl) {
    GtkWidget *dlg(gtk_file_chooser_dialog_new(fmt("Select %0 Folder", lbl).c_str(),
					       GTK_WINDOW(window),
					       GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
					       "_Cancel", GTK_RESPONSE_CANCEL,
					       "_Select", GTK_RESPONSE_ACCEPT,
					       nullptr));
    const char *f(gtk_entry_get_text(e));
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dlg), f);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
      char *dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_entry_set_text(e, dir);
      g_free(dir);
    }

    gtk_widget_destroy(dlg);
  }
  
  static void on_lfolder(gpointer *_, Setup *v) {
    on_folder(GTK_ENTRY(v->load_folder), "Load");
  }

  static void on_sfolder(gpointer *_, Setup *v) {
    on_folder(GTK_ENTRY(v->save_folder), "Load");
  }

  static void on_cancel(gpointer *_, Setup *v) {
    log(v->ctx, "Cancelled setup");
    pop_view(v);
  }

  static void copy_flds(Server &v) {
    Ctx &ctx(v.ctx);
    set_val(v.settings.url, get_str(GTK_ENTRY(v.url)));
    auto port_str(get_str(GTK_ENTRY(v.port)));
    auto port(to_int64(port_str));
    
    if (!port) {
      log(ctx, fmt("Invalid port: %0", port_str));
    } else {
      set_val(v.settings.port, port);
    }

    set_val(v.settings.user, get_str(GTK_ENTRY(v.user)));
    set_val(v.settings.pass, get_str(GTK_ENTRY(v.pass)));
    auto poll_str(get_str(GTK_ENTRY(v.poll)));
    set_val(v.settings.poll, to_int64(poll_str));
  }
  
  static void on_save(gpointer *_, Setup *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    TRY(try_save);
    
    Peer &me(whoami(ctx));
    me.name = gtk_entry_get_text(GTK_ENTRY(v->name));
    me.email = gtk_entry_get_text(GTK_ENTRY(v->email));
    update(ctx.db.peers, me);

    if (!try_save.errors.empty()) { return; }
    auto pass(get_str(GTK_ENTRY(v->pass)));
    
    if (!pass.empty()) {
      if (get_str(GTK_ENTRY(v->pass_repeat)) != pass) {
	log(ctx, "Password mismatch");
	return;
      }

      log(ctx, "Rewriting database...");
      remove_path(get_path(ctx, "pass"));
      init_pass(ctx, pass);
      db::defrag(ctx);
      if (!try_save.errors.empty()) { return; }
      log(ctx, "Password changed");
    }
    
    set_val(ctx.settings.load_folder,
	    str(gtk_entry_get_text(GTK_ENTRY(v->load_folder))));
    set_val(ctx.settings.save_folder,
	    str(gtk_entry_get_text(GTK_ENTRY(v->save_folder))));

    copy_flds(v->imap);
    if (*get_val(ctx.settings.imap.poll)) {
      ctx.fetch_cond.notify_one();
    }
    
    copy_flds(v->smtp);
    if (*get_val(ctx.settings.smtp.poll)) {
      ctx.send_cond.notify_one();
    }

    if (try_save.errors.empty()) {
      db::commit(trans, "Saved Setup");
      log(v->ctx, "Saved setup");
      pop_view(v);
    }
  }
  
  static void on_imap(gpointer *_, Server *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(v->ctx);
    copy_flds(*v);

    TRY(try_imap);
    Imap imap(ctx);
    if (try_imap.errors.empty()) { log(ctx, "Imap Ok"); }
  }

  static void on_smtp(gpointer *_, Server *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(v->ctx);
    copy_flds(*v);

    TRY(try_smtp);
    Smtp smtp(ctx);
    if (try_smtp.errors.empty()) { log(ctx, "Smtp Ok"); }
  }

  static GtkWidget *init_folder(Setup &v,
				GtkWidget *e,
				GCallback fn,
				const str &lbl) {
    GtkWidget *frm(new_grid());

    gtk_grid_attach(GTK_GRID(frm), new_label(fmt("%0 Folder", lbl)), 0, 0, 1, 1);
    gtk_widget_set_hexpand(e, true);
    gtk_widget_set_sensitive(e, false);
    gtk_grid_attach(GTK_GRID(frm), e, 0, 1, 1, 1);

    GtkWidget *btn = gtk_button_new_with_label("Select");
    g_signal_connect(btn, "clicked", fn, &v);
    gtk_grid_attach(GTK_GRID(frm), btn, 1, 1, 1, 1);
    return frm;
  }
  
  static GtkWidget *init_general(Setup &v) {
    GtkWidget *frm(new_grid());
    gtk_widget_set_margin_top(frm, 5);
    int row(0);
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Name"), 0, row, 1, 1);
    gtk_widget_set_hexpand(v.name, true);
    gtk_grid_attach(GTK_GRID(frm), v.name, 0, row+1, 1, 1);
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Email"), 1, row, 1, 1);
    gtk_widget_set_hexpand(v.email, true);
    gtk_grid_attach(GTK_GRID(frm), v.email, 1, row+1, 1, 1);

    row += 2;
    auto lbl(new_label("Change Password"));
    gtk_widget_set_margin_top(lbl, 5);
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, row, 1, 1);
    gtk_entry_set_visibility(GTK_ENTRY(v.pass), false);
    gtk_grid_attach(GTK_GRID(frm), v.pass, 0, row+1, 1, 1);    
    lbl = new_label("Repeat");
    gtk_widget_set_margin_top(lbl, 5);
    gtk_grid_attach(GTK_GRID(frm), lbl, 1, row, 1, 1);
    gtk_entry_set_visibility(GTK_ENTRY(v.pass_repeat), false);
    gtk_grid_attach(GTK_GRID(frm), v.pass_repeat, 1, row+1, 1, 1);

    row += 2;
    auto lf(init_folder(v, v.load_folder, G_CALLBACK(on_lfolder), "Load"));
    gtk_widget_set_margin_top(lf, 10);
    gtk_grid_attach(GTK_GRID(frm), lf, 0, row, 1, 1);
    auto sf(init_folder(v, v.save_folder, G_CALLBACK(on_sfolder), "Save"));
    gtk_widget_set_margin_top(sf, 10);
    gtk_grid_attach(GTK_GRID(frm), sf, 1, row, 1, 1);
    
    return frm;
  }
      
  Setup::Setup(Ctx &ctx):
    View(ctx, "Setup"),
    name(gtk_entry_new()),
    email(gtk_entry_new()),
    pass(gtk_entry_new()),
    pass_repeat(gtk_entry_new()),
    load_folder(gtk_entry_new()),
    save_folder(gtk_entry_new()),
    save(gtk_button_new_with_mnemonic("_Save Setup")),
    cancel(gtk_button_new_with_mnemonic("_Cancel")),
    imap(ctx, ctx.settings.imap, G_CALLBACK(on_imap)),
    smtp(ctx, ctx.settings.smtp, G_CALLBACK(on_smtp))
  {
    GtkWidget *tabs(gtk_notebook_new());
    gtk_widget_set_vexpand(tabs, true);
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_general(*this),
			     gtk_label_new_with_mnemonic("_1 General"));
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     imap.box,
			     gtk_label_new_with_mnemonic("_2 Imap"));
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     smtp.box,
			     gtk_label_new_with_mnemonic("_3 Smtp"));
    gtk_container_add(GTK_CONTAINER(panel), tabs);
    
    GtkWidget *btns(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
    
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);    
    focused = name;
  }

  static void load_flds(Server &v) {
    set_str(GTK_ENTRY(v.url), *get_val(v.settings.url));
    set_str(GTK_ENTRY(v.port), to_str(*get_val(v.settings.port)));
    set_str(GTK_ENTRY(v.user), *get_val(v.settings.user));
    set_str(GTK_ENTRY(v.pass), *get_val(v.settings.pass));
    set_str(GTK_ENTRY(v.poll), to_str(*get_val(v.settings.poll)));
  }
  
  void Setup::load() {
    View::load();
    
    Peer &me(whoami(ctx));
    set_str(GTK_ENTRY(name), me.name);
    set_str(GTK_ENTRY(email), me.email);

    set_str(GTK_ENTRY(load_folder), *get_val(ctx.settings.load_folder));    
    set_str(GTK_ENTRY(save_folder), *get_val(ctx.settings.save_folder));

    load_flds(imap);
    load_flds(smtp);
  }
}}

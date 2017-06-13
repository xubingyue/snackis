#include "snackis/ctx.hpp"
#include "snackis/gui/setup.hpp"

namespace snackis {
namespace gui {
  static void on_cancel(GtkWidget *_, Setup *setup) {
    log(setup->ctx, "Cancelled setup");
    setup->pop_view();
  }

  static void on_save(GtkWidget *_, Setup *setup) {
    Ctx &ctx(setup->ctx);
    
    db::Trans trans(ctx);
    Peer &me(whoami(ctx));
    me.name = gtk_entry_get_text(GTK_ENTRY(setup->name));
    me.email = gtk_entry_get_text(GTK_ENTRY(setup->email));
    update(ctx.db.peers, me);

    set_val(ctx.settings.load_folder,
	    str(gtk_entry_get_text(GTK_ENTRY(setup->load_folder))));
    set_val(ctx.settings.save_folder,
	    str(gtk_entry_get_text(GTK_ENTRY(setup->save_folder))));

    db::commit(trans);
    log(setup->ctx, "Saved setup");
    setup->pop_view();
  }

  Setup::Setup(Ctx &ctx):
    View(ctx, "Setup"),
    name(gtk_entry_new()),
    email(gtk_entry_new()),
    load_folder(gtk_entry_new()),
    save_folder(gtk_entry_new()) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    Peer &me(whoami(ctx));

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(name, true);
    gtk_entry_set_text(GTK_ENTRY(name), me.name.c_str());
    gtk_container_add(GTK_CONTAINER(frm), name);
    
    lbl = gtk_label_new("Email");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(email, true);
    gtk_entry_set_text(GTK_ENTRY(email), me.email.c_str());
    gtk_container_add(GTK_CONTAINER(frm), email);

    lbl = gtk_label_new("Load-folder");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_widget_set_margin_top(lbl, 5);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(load_folder, true);
    gtk_entry_set_text(GTK_ENTRY(load_folder),
		       get_val(ctx.settings.load_folder)->c_str());
    gtk_container_add(GTK_CONTAINER(frm), load_folder);

    lbl = gtk_label_new("Save-folder");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(save_folder, true);
    gtk_entry_set_text(GTK_ENTRY(save_folder),
		       get_val(ctx.settings.save_folder)->c_str());
    gtk_container_add(GTK_CONTAINER(frm), save_folder);
    
    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    cancel = gtk_button_new_with_mnemonic("_Cancel");
    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    
    save = gtk_button_new_with_mnemonic("_Save");
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);
  }

  void Setup::focus() {
    gtk_widget_grab_focus(name);
  }
}}

#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/login.hpp"
#include "snackis/gui/reader.hpp"

namespace snackis {
namespace gui {
  static void on_login(GtkWidget *_, Login *login) {
    login->pop_view();
    Ctx &ctx(login->ctx);
    
    open(ctx);
    Peer &me(whoami(ctx));
    if (!me.name.empty()) { log(ctx, fmt("Welcome back, %0", me.name)); }
    
    gui::reader.emplace();
    gtk_box_pack_start(GTK_BOX(left_panel), ptr(*reader), false, false, 0);
    focus(*gui::reader);
    gtk_widget_show_all(gui::window);
  }
  
  Login::Login(Ctx &ctx):
    View(ctx),
    pass(gtk_entry_new()) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 10);
    gtk_widget_set_halign(frm, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(frm, GTK_ALIGN_CENTER);
    
    lbl = gtk_label_new("Password: ");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(frm), lbl, false, false, 0);

    gtk_widget_set_hexpand(pass, true);
    gtk_entry_set_visibility(GTK_ENTRY(pass), false);
    g_signal_connect(pass, "activate", G_CALLBACK(on_login), this);
    gtk_box_pack_start(GTK_BOX(frm), pass, false, false, 0);

    lbl = gtk_label_new("Repeat: ");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(frm), lbl, false, false, 0);

    repeat = gtk_entry_new();
    gtk_widget_set_hexpand(repeat, true);
    gtk_entry_set_visibility(GTK_ENTRY(repeat), false);
    g_signal_connect(repeat, "activate", G_CALLBACK(on_login), this);
    gtk_box_pack_start(GTK_BOX(frm), repeat, false, false, 0);
  }

  void Login::focus() {
    gtk_widget_grab_focus(pass);
  }
}}

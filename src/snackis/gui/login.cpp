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
    View(ctx, gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)),
    pass(gtk_entry_new()) {
    GtkWidget *lbl = gtk_label_new("Login");
    add_style(lbl, "view_label");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(panel), lbl);
    g_signal_connect(pass, "activate", G_CALLBACK(on_login), this);
    gtk_container_add(GTK_CONTAINER(panel), pass);
    push_view();
  }

  void Login::focus() {
    gtk_widget_grab_focus(pass);
  }
}}

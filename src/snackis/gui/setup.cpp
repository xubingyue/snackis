#include "snackis/ctx.hpp"
#include "snackis/gui/setup.hpp"

namespace snackis {
namespace gui {
  static void on_cancel(GtkWidget *_, Setup *setup) {
    setup->pop_view();
  }

  static void on_save(GtkWidget *_, Setup *setup) {
  }

  Setup::Setup(Ctx &ctx):
    View(ctx, "Setup"),
    name(gtk_entry_new()),
    email(gtk_entry_new()) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    lbl = gtk_label_new("Name");
    gtk_widget_set_margin_top(lbl, 5);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 1, 1);
    gtk_widget_set_margin_top(name, 5);
    gtk_widget_set_hexpand(name, true);
    gtk_grid_attach(GTK_GRID(frm), name, 0, 1, 1, 1);
    
    lbl = gtk_label_new("Email");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_widget_set_margin_top(lbl, 5);
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 2, 1, 1);
    gtk_widget_set_margin_top(email, 5);
    gtk_widget_set_hexpand(email, true);
    gtk_grid_attach(GTK_GRID(frm), email, 0, 3, 1, 1);
    
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

#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/encrypt.hpp"

namespace snackis {
namespace gui {
  static void on_source(gpointer *_, Encrypt *enc) {
    GtkWidget *dlg(gtk_file_chooser_dialog_new("Select Load Folder",
					       GTK_WINDOW(window),
					       GTK_FILE_CHOOSER_ACTION_OPEN,
					       "_Cancel", GTK_RESPONSE_CANCEL,
					       "_Select", GTK_RESPONSE_ACCEPT,
					       nullptr));
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dlg),
				  gtk_entry_get_text(GTK_ENTRY(enc->source)));

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
      char *dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_entry_set_text(GTK_ENTRY(enc->source), dir);
      g_free(dir);
      gtk_widget_destroy(dlg);
    }
  }

  static void on_target(gpointer *_, Encrypt *enc) {
    GtkWidget *dlg(gtk_file_chooser_dialog_new("Select Save Folder",
					       GTK_WINDOW(window),
					       GTK_FILE_CHOOSER_ACTION_SAVE,
					       "_Cancel", GTK_RESPONSE_CANCEL,
					       "_Select", GTK_RESPONSE_ACCEPT,
					       nullptr));
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dlg),
				  gtk_entry_get_text(GTK_ENTRY(enc->target)));

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
      char *dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_entry_set_text(GTK_ENTRY(enc->target), dir);
      g_free(dir);
      gtk_widget_destroy(dlg);
    }
  }

  static void on_cancel(gpointer *_, Encrypt *enc) {
    log(enc->ctx, "Cancelled encryption");
    enc->pop_view();
  }

  static void on_save(gpointer *_, Encrypt *enc) {
    Ctx &ctx(enc->ctx);
    db::Trans trans(ctx);
    db::commit(trans);
    log(enc->ctx, "Saved encrypted file");
    enc->pop_view();
  }
  
  static GtkWidget *init_source(Encrypt &enc) {
    Ctx &ctx(enc.ctx);
    GtkWidget *frm = gtk_grid_new();
    gtk_widget_set_margin_top(frm, 5);
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    GtkWidget *lbl = gtk_label_new("Load Folder");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 1, 1);
    gtk_widget_set_hexpand(enc.source, true);
    gtk_editable_set_editable(GTK_EDITABLE(enc.source), false);
    gtk_widget_set_can_focus(enc.source, false);
    gtk_entry_set_text(GTK_ENTRY(enc.source),
		       get_val(ctx.settings.load_folder)->c_str());
    gtk_grid_attach(GTK_GRID(frm), enc.source, 0, 1, 1, 1);

    GtkWidget *btn = gtk_button_new_with_label("Select Folder");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_source), &enc);
    gtk_grid_attach(GTK_GRID(frm), btn, 1, 1, 1, 1);
    return frm;
  }

  static GtkWidget *init_target(Encrypt &enc) {
    Ctx &ctx(enc.ctx);
    GtkWidget *frm = gtk_grid_new();
    gtk_widget_set_margin_top(frm, 5);
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    GtkWidget *lbl = gtk_label_new("Save Folder");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 1, 1);
    gtk_widget_set_hexpand(enc.target, true);
    gtk_editable_set_editable(GTK_EDITABLE(enc.target), false);
    gtk_widget_set_can_focus(enc.target, false);
    gtk_entry_set_text(GTK_ENTRY(enc.target),
		       get_val(ctx.settings.save_folder)->c_str());
    gtk_grid_attach(GTK_GRID(frm), enc.target, 0, 1, 1, 1);

    GtkWidget *btn = gtk_button_new_with_label("Select Folder");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_target), &enc);
    gtk_grid_attach(GTK_GRID(frm), btn, 1, 1, 1, 1);
    return frm;
  }

  Encrypt::Encrypt(Ctx &ctx):
    View(ctx, "Encrypt"),
    peer(gtk_combo_box_new()),
    source(gtk_entry_new()),
    target(gtk_entry_new()),
    encode(gtk_check_button_new_with_label("Encode")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(peer, true);
    gtk_container_add(GTK_CONTAINER(frm), peer);
    
    gtk_container_add(GTK_CONTAINER(frm), init_source(*this));
    gtk_container_add(GTK_CONTAINER(frm), init_target(*this));
    gtk_container_add(GTK_CONTAINER(frm), encode);

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    cancel = gtk_button_new_with_mnemonic("_Cancel Encryption");
    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    
    save = gtk_button_new_with_mnemonic("_Save Encrypted File");
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);
  }
  
  void Encrypt::focus() {
    gtk_widget_grab_focus(peer);
  }
}}

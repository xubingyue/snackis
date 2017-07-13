#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/decrypt.hpp"

namespace snackis {
namespace gui {
  static void refresh(Decrypt &v) {
    auto peer_sel(v.peer_fld.selected);
    auto source_sel(str(gtk_entry_get_text(GTK_ENTRY(v.source))));
    auto target_sel(str(gtk_entry_get_text(GTK_ENTRY(v.target))));
    
    gtk_widget_set_sensitive(v.save_btn,
			     v.peer_fld.selected &&
			     !source_sel.empty() &&
			     !target_sel.empty());
  }

  static void on_source(gpointer *_, Decrypt *v) {
    Ctx &ctx(v->ctx);
    GtkWidget *dlg(gtk_file_chooser_dialog_new("Select Source File",
					       GTK_WINDOW(window),
					       GTK_FILE_CHOOSER_ACTION_OPEN,
					       "_Cancel", GTK_RESPONSE_CANCEL,
					       "_Select", GTK_RESPONSE_ACCEPT,
					       nullptr));

    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg),
					get_val(ctx.settings.load_folder)->c_str());

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
      char *dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_entry_set_text(GTK_ENTRY(v->source), dir);
      g_free(dir);
      refresh(*v);
    }

    gtk_widget_destroy(dlg);
  }

  static void on_target(gpointer *_, Decrypt *v) {
    Ctx &ctx(v->ctx);
    GtkWidget *dlg(gtk_file_chooser_dialog_new("Select Target File",
					       GTK_WINDOW(window),
					       GTK_FILE_CHOOSER_ACTION_SAVE,
					       "_Cancel", GTK_RESPONSE_CANCEL,
					       "_Select", GTK_RESPONSE_ACCEPT,
					       nullptr));

    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg),
					get_val(ctx.settings.save_folder)->c_str());
    Path src(Path(gtk_entry_get_text(GTK_ENTRY(v->source))));
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dlg), src.filename().c_str());

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
      char *dir = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
      gtk_entry_set_text(GTK_ENTRY(v->target), dir);
      g_free(dir);
      refresh(*v);
    }

    gtk_widget_destroy(dlg);
  }

  static void on_cancel(gpointer *_, Decrypt *v) {
    log(v->ctx, "Cancelled decryption");
    pop_view(v);
    delete v;
  }

  static void on_save(gpointer *_, Decrypt *v) {
    Ctx &ctx(v->ctx);

    const str
      in_path(gtk_entry_get_text(GTK_ENTRY(v->source))),
      out_path(gtk_entry_get_text(GTK_ENTRY(v->target)));
    
    log(ctx, fmt("Decrypting '%0' to '%1'...", in_path, out_path));

    TRY(try_decrypt);
    decrypt(*v->peer_fld.selected,
	    in_path, out_path,
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->decode)));
    
    if (try_decrypt.errors.empty()) {
      log(v->ctx, "Finished decrypting");
      pop_view(v);
      delete v;
    }
  }

  static GtkWidget *init_source(Decrypt &v) {
    GtkWidget *frm = gtk_grid_new();
    gtk_widget_set_margin_top(frm, 5);
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_grid_attach(GTK_GRID(frm), new_label("Source File"), 0, 0, 1, 1);
    gtk_widget_set_hexpand(v.source, true);
    gtk_widget_set_sensitive(v.source, false);
    gtk_grid_attach(GTK_GRID(frm), v.source, 0, 1, 1, 1);

    GtkWidget *btn = gtk_button_new_with_label("Select");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_source), &v);
    gtk_grid_attach(GTK_GRID(frm), btn, 1, 1, 1, 1);
    return frm;
  }

  static GtkWidget *init_target(Decrypt &v) {
    GtkWidget *frm = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_grid_attach(GTK_GRID(frm), new_label("Target File"), 0, 0, 1, 1);
    gtk_widget_set_hexpand(v.target, true);
    gtk_widget_set_sensitive(v.target, false);
    gtk_grid_attach(GTK_GRID(frm), v.target, 0, 1, 1, 1);

    GtkWidget *btn = gtk_button_new_with_label("Select");
    g_signal_connect(btn, "clicked", G_CALLBACK(on_target), &v);
    gtk_grid_attach(GTK_GRID(frm), btn, 1, 1, 1, 1);
    return frm;
  }

  Decrypt::Decrypt(Ctx &ctx):
    View(ctx, "Decrypt"),
    source(gtk_entry_new()),
    target(gtk_entry_new()),
    decode(gtk_check_button_new_with_label("Decode")),
    save_btn(gtk_button_new_with_mnemonic("_Save Decrypted File")),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel")),
    peer_fld(ctx)
  {
    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    gtk_container_add(GTK_CONTAINER(frm), new_label("Peer"));
    peer_fld.on_change.emplace([this]() { refresh(*this); });
    gtk_container_add(GTK_CONTAINER(frm), peer_fld.ptr());
    
    gtk_container_add(GTK_CONTAINER(frm), init_source(*this));

    GtkWidget *decode_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_container_add(GTK_CONTAINER(frm), decode_box);
    gtk_container_add(GTK_CONTAINER(decode_box), decode);
    
    gtk_container_add(GTK_CONTAINER(frm), init_target(*this));

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save_btn);

    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel_btn);
    focused = peer_fld.search_btn;
    refresh(*this);    
  }
}}

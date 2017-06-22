#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/decrypt.hpp"

namespace snackis {
namespace gui {
  enum PeerCol {COL_PEER_PTR=0, COL_PEER_NAME};
    
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
    }

    gtk_widget_destroy(dlg);
  }

  static void on_cancel(gpointer *_, Decrypt *v) {
    log(v->ctx, "Cancelled decryption");
    pop_view(*v);
  }

  static void on_save(gpointer *_, Decrypt *v) {
    Ctx &ctx(v->ctx);
    const str
      in_path(gtk_entry_get_text(GTK_ENTRY(v->source))),
      out_path(gtk_entry_get_text(GTK_ENTRY(v->target)));
	    
    log(ctx, fmt("Decrypting '%0' to '%1'...", in_path, out_path));
    GtkTreeIter iter;
    gtk_combo_box_get_active_iter(GTK_COMBO_BOX(v->peer), &iter);
    db::Rec<Peer> *peer_rec;
    gtk_tree_model_get(GTK_TREE_MODEL(v->peers),
		       &iter,
		       COL_PEER_PTR, &peer_rec,
		       -1);
    Peer peer(ctx, *peer_rec);
    
    decrypt(peer,
	    in_path, out_path,
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(v->decode)));
    
    log(v->ctx, "Finished decrypting");
    pop_view(*v);
  }

  void init_peers(Decrypt &v) {
    Ctx &ctx(v.ctx);
    
    for(const auto &peer_rec: ctx.db.peers.recs) {
      Peer peer(ctx, peer_rec);
      
      GtkTreeIter iter;
      gtk_list_store_append(v.peers, &iter);
      gtk_list_store_set(v.peers, &iter,
			 COL_PEER_PTR, &peer_rec,
			 COL_PEER_NAME, peer.name.c_str(),
			 -1);
    }
    
    auto col(gtk_cell_renderer_text_new());
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(v.peer), col, true);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(v.peer),
				   col,
                                   "text", COL_PEER_NAME,
				   nullptr);
    gtk_combo_box_set_active(GTK_COMBO_BOX(v.peer), 0);
  }
  
  static GtkWidget *init_source(Decrypt &v) {
    GtkWidget *frm = gtk_grid_new();
    gtk_widget_set_margin_top(frm, 5);
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    GtkWidget *lbl = gtk_label_new("Source File");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 1, 1);
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
    gtk_widget_set_margin_top(frm, 5);
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    GtkWidget *lbl = gtk_label_new("Target File");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);  
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 1, 1);
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
    peers(gtk_list_store_new(2, G_TYPE_POINTER, G_TYPE_STRING)),
    peer(gtk_combo_box_new_with_model(GTK_TREE_MODEL(peers))),
    source(gtk_entry_new()),
    target(gtk_entry_new()),
    decode(gtk_check_button_new_with_label("Decode")),
    save(gtk_button_new_with_mnemonic("_Save Decrypted File")),
    cancel(gtk_button_new_with_mnemonic("_Cancel")) {
    GtkWidget *lbl;

    GtkWidget *frm = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(panel), frm, true, true, 0);
    
    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(frm), lbl);
    gtk_widget_set_hexpand(peer, true);
    gtk_container_add(GTK_CONTAINER(frm), peer);
    init_peers(*this);
    
    gtk_container_add(GTK_CONTAINER(frm), init_source(*this));

    GtkWidget *decode_box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_container_add(GTK_CONTAINER(frm), decode_box);
    gtk_container_add(GTK_CONTAINER(decode_box), decode);
    
    gtk_container_add(GTK_CONTAINER(frm), init_target(*this));

    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
    focused = peer;
  }
}}

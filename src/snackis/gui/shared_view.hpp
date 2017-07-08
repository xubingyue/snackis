#ifndef SNACKIS_GUI_SHARED_VIEW_HPP
#define SNACKIS_GUI_SHARED_VIEW_HPP

#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct SharedView: RecView<RecT> {
    GtkWidget *owner_id_fld, *owner_name_fld, *created_at_fld, *changed_at_fld;
    
    SharedView(const str &lbl, const RecT &rec);
  };

  template <typename RecT>
  SharedView<RecT>::SharedView(const str &label, const RecT &rec):
    RecView<RecT>(label, rec),
    owner_id_fld(new_id_field()),
    owner_name_fld(gtk_entry_new()),
    created_at_fld(gtk_entry_new()),
    changed_at_fld(gtk_entry_new())
  {
    GtkWidget *hdr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(hdr), true);    
    gtk_container_add(GTK_CONTAINER(this->fields), hdr);

    GtkWidget *frm = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_container_add(GTK_CONTAINER(hdr), frm);

    Peer own(get_peer_id(rec.ctx, rec.owner_id));
    
    GtkWidget *lbl;
    lbl = new_label("Owner");
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 2, 1);
    gtk_widget_set_sensitive(owner_id_fld, false);
    gtk_grid_attach(GTK_GRID(frm), owner_id_fld, 0, 1, 1, 1);
    set_str(GTK_ENTRY(owner_id_fld), id_str(own));
    gtk_widget_set_sensitive(owner_name_fld, false);
    gtk_grid_attach(GTK_GRID(frm), owner_name_fld, 1, 1, 1, 1);
    set_str(GTK_ENTRY(owner_name_fld), own.name);
    
    frm = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_container_add(GTK_CONTAINER(hdr), frm);

    lbl = new_label("Created At");
    gtk_grid_attach(GTK_GRID(frm), lbl, 0, 0, 1, 1);
    gtk_widget_set_sensitive(created_at_fld, false);
    gtk_grid_attach(GTK_GRID(frm), created_at_fld, 0, 1, 1, 1);
    set_str(GTK_ENTRY(created_at_fld), fmt(rec.created_at, "%a %b %d, %H:%M"));

    lbl = new_label("Changed At");
    gtk_grid_attach(GTK_GRID(frm), lbl, 1, 0, 1, 1);    
    gtk_widget_set_sensitive(changed_at_fld, false);
    gtk_grid_attach(GTK_GRID(frm), changed_at_fld, 1, 1, 1, 1);    
    set_str(GTK_ENTRY(changed_at_fld), fmt(rec.changed_at, "%a %b %d, %H:%M"));
  }
}}

#endif

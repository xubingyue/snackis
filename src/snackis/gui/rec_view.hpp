#ifndef SNACKIS_GUI_REC_VIEW_HPP
#define SNACKIS_GUI_REC_VIEW_HPP

#include <mutex>
#include <vector>

#include "snackis/rec.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct RecView: View {
    RecT rec;
    GtkWidget *fields, *save, *cancel;
    RecView(const str &lbl, const RecT &rec);
    virtual void on_save()=0;
  };

  template <typename RecT>
  void on_cancel_rec(gpointer *_, RecView<RecT> *v) {
    log(v->ctx, fmt("Cancelled %0", gtk_label_get_text(GTK_LABEL(v->label))));
    pop_view(*v);
  }

  template <typename RecT>
  void on_save_rec(gpointer *_, RecView<RecT> *v) {
    Ctx &ctx(v->ctx);
    db::Trans trans(ctx);
    v->on_save();
    db::commit(trans);
    log(ctx, fmt("Saved %0", gtk_label_get_text(GTK_LABEL(v->label))));
    pop_view(*v);
  }
  
  template <typename RecT>
  RecView<RecT>::RecView(const str &lbl, const RecT &rec):
    View(rec.ctx, lbl, to_str(rec.id)),
    rec(rec),
    fields(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    save(gtk_button_new_with_mnemonic(fmt("_Save %0", lbl).c_str())),
    cancel(gtk_button_new_with_mnemonic("_Cancel"))
  {
    gtk_box_pack_start(GTK_BOX(panel), fields, true, true, 0);
    
    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save, "clicked", G_CALLBACK(on_save_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(btns), save);

    g_signal_connect(cancel, "clicked", G_CALLBACK(on_cancel_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel);
  }
}}

#endif

#ifndef SNACKIS_GUI_REC_VIEW_HPP
#define SNACKIS_GUI_REC_VIEW_HPP

#include "snackis/rec.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct RecView: View {
    RecT rec;
    GtkWidget *fields, *save_btn, *cancel_btn;
    RecView(const str &lbl, const RecT &rec);
    virtual bool allow_save() const;
    virtual bool save()=0;
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

    if (v->save()) {
      db::commit(trans);
      log(ctx, fmt("Saved %0", gtk_label_get_text(GTK_LABEL(v->label))));
      pop_view(*v);
    }
  }
  
  template <typename RecT>
  RecView<RecT>::RecView(const str &lbl, const RecT &rec):
    View(rec.ctx, lbl, id_str(rec)),
    rec(rec),
    fields(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    save_btn(gtk_button_new_with_mnemonic(fmt("_Save %0", lbl).c_str())),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel"))
  {
    gtk_box_pack_start(GTK_BOX(panel), fields, true, true, 0);
    
    GtkWidget *btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_top(btns, 10);
    gtk_widget_set_halign(btns, GTK_ALIGN_END);
    gtk_widget_set_valign(btns, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), btns);
        
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(btns), save_btn);

    g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel_rec<RecT>), this);
    gtk_container_add(GTK_CONTAINER(btns), cancel_btn);
  }

  template <typename RecT>
  bool RecView<RecT>::allow_save() const { return true; }

  template <typename RecT>
  void refresh(RecView<RecT> &v) {
    gtk_widget_set_sensitive(v.save_btn, v.allow_save());
  }
}}

#endif

#ifndef SNACKIS_GUI_REC_VIEW_HPP
#define SNACKIS_GUI_REC_VIEW_HPP

#include "snackis/ctx.hpp"
#include "snackis/rec.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct RecView: View {
    using EventFn = func<void ()>;
    using Event = std::vector<EventFn>;
    
    RecT rec;
    GtkWidget *menu, *fields, *save_btn, *cancel_btn;
    Event on_save;
    Event on_cancel;
    
    RecView(const str &lbl, const RecT &rec);
    virtual bool allow_save() const;
    virtual bool save()=0;
  };

  template <typename RecT>
  void on_cancel_rec(gpointer *_, RecView<RecT> *v) {
    Ctx &ctx(v->ctx);

    if (!v->on_cancel.empty()) {
      TRY(try_save);
      db::Trans trans(ctx);
      for (auto fn: v->on_cancel) { fn(); }
      if (try_save.errors.empty()) {
	db::commit(trans, fmt("Cancelled %0", label(*v)));
      }
    }

    log(ctx, fmt("Cancelled %0", gtk_label_get_text(GTK_LABEL(v->label))));
    pop_view(*v);
    
  }

  template <typename RecT>
  void on_save_rec(gpointer *_, RecView<RecT> *v) {
    Ctx &ctx(v->ctx);
    TRY(try_save);
    db::Trans trans(ctx);
    
    if (v->save() && try_save.errors.empty()) {
      log(ctx, fmt("Saved %0", gtk_label_get_text(GTK_LABEL(v->label))));
      pop_view(*v);
      for (auto fn: v->on_save) { fn(); }
      db::commit(trans, fmt("Saved %0", label(*v)));
    }
  }
  
  template <typename RecT>
  RecView<RecT>::RecView(const str &lbl, const RecT &rec):
    View(rec.ctx, lbl, id_str(rec)),
    rec(rec),
    menu(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5)),
    fields(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)),
    save_btn(gtk_button_new_with_mnemonic(fmt("_Save %0", lbl).c_str())),
    cancel_btn(gtk_button_new_with_mnemonic("_Cancel"))
  {
    gtk_widget_set_margin_bottom(menu, 5);
    gtk_container_add(GTK_CONTAINER(panel), menu);
    gtk_widget_set_vexpand(fields, true);
    gtk_container_add(GTK_CONTAINER(panel), fields);
    
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

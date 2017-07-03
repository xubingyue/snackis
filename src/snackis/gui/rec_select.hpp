#ifndef SNACKIS_GUI_REC_SELECT_HPP
#define SNACKIS_GUI_REC_SELECT_HPP

#include "snackis/gui/search_view.hpp"

namespace snackis {
namespace gui {
  template <typename RecT>
  struct RecSelect: Widget {
    Ctx &ctx;
    GtkWidget *box, *id_fld, *name_fld, *search_btn, *clear_btn;
    opt<RecT> selected;
    
    RecSelect(Ctx &ctx);
    GtkWidget *ptr() override;
    virtual SearchView<RecT> *search() const=0;
  };

  template <typename RecT>
  void select(RecSelect<RecT> &v, const opt<RecT> &rec) {
    if (rec) {
      gtk_entry_set_text(GTK_ENTRY(v.id_fld), id_str(*rec).c_str());
      gtk_entry_set_text(GTK_ENTRY(v.name_fld), rec->name.c_str());
      gtk_widget_set_sensitive(v.clear_btn, true);
      v.selected.emplace(*rec);
    } else {
      gtk_entry_set_text(GTK_ENTRY(v.id_fld), "n/a");
      gtk_entry_set_text(GTK_ENTRY(v.name_fld), "");
      v.selected.reset();
    }
  }

  template <typename RecT>
  void on_rec_search(gpointer *_, RecSelect<RecT> *v) {
    auto sv(v->search());
    
    sv->on_activate = [v, sv](auto &rec) {
      select<RecT>(*v, RecT(v->ctx, rec));
      pop_view(*sv);
    };
    
    push_view(*sv);
  }

  template <typename RecT>
  void on_rec_clear(gpointer *_, RecSelect<RecT> *v) {
    select<RecT>(*v, nullopt);
  }

  template <typename RecT>
  RecSelect<RecT>::RecSelect(Ctx &ctx):
    ctx(ctx),
    box(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5)),
    id_fld(gtk_entry_new()),
    name_fld(gtk_entry_new()),
    search_btn(gtk_button_new_with_mnemonic("Search")),
    clear_btn(gtk_button_new_with_mnemonic("Clear"))
  {
    gtk_widget_set_sensitive(id_fld, false);
    gtk_container_add(GTK_CONTAINER(box), id_fld);
    gtk_widget_set_sensitive(name_fld, false);
    gtk_widget_set_hexpand(name_fld, true);
    gtk_container_add(GTK_CONTAINER(box), name_fld);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_rec_search<RecT>), this);
    gtk_container_add(GTK_CONTAINER(box), search_btn);
    gtk_widget_set_sensitive(clear_btn, false);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(on_rec_clear<RecT>), this);
    gtk_container_add(GTK_CONTAINER(box), clear_btn);
    select<RecT>(*this, nullopt);
  }

  template <typename RecT>
  GtkWidget *RecSelect<RecT>::ptr() { return box; }
}}

#endif

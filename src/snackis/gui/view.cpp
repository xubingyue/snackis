#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  std::list<View *> View::stack;

  static gboolean on_key(gpointer _, GdkEventKey *ev, View *v) {
    if (ev->keyval == GDK_KEY_F5) {
      refresh(v->ctx);
      v->load();
      const str lbl(gtk_label_get_text(GTK_LABEL(v->label)));
      log(v->ctx, fmt("Refreshed %0", lbl));
      return true;
    }
    
    return false;
  }
  
  View::View(Ctx &ctx, const str &lbl): 
    ctx(ctx),
    panel(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)),
    label(new_label(lbl)),
    menu(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5)),
    focused(panel),
    visible(false) { 
    add_style(panel, "view");
    gtk_widget_set_margin_start(panel, 5);
    gtk_widget_set_margin_end(panel, 5);
    gtk_widget_set_margin_top(panel, 5);
    gtk_widget_set_margin_bottom(panel, 5);

    auto hdr(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_widget_set_margin_bottom(hdr, 10);
    gtk_container_add(GTK_CONTAINER(panel), hdr);
    
    add_style(label, "view_label");
    gtk_widget_set_hexpand(label, true);
    gtk_widget_set_valign(label, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(hdr), label);

    gtk_widget_set_valign(menu, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(hdr), menu);
    
    g_signal_connect(G_OBJECT(panel),
		     "key_release_event",
		     G_CALLBACK(on_key),
		     this);
  }

  View::~View() {
    if (!visible) { gtk_widget_destroy(panel); }
  }

  void View::load()
  { }

  GtkWidget *View::ptr() {
      return panel;
  }

  void push_view(View *v) {
    if (!View::stack.empty()) {
      View *prev(View::stack.back());
      prev->focused = gtk_window_get_focus(GTK_WINDOW(window));
      g_object_ref(prev->panel);
      prev->visible = false;
      gtk_container_remove(GTK_CONTAINER(panels), prev->panel);
    }

    v->load();
    View::stack.push_back(v);    
    v->visible = true;
    gtk_container_add(GTK_CONTAINER(panels), v->panel);  
    gtk_widget_show_all(v->ptr());
    gtk_widget_grab_focus(v->focused);
  }

  void pop_view(View *v) {
    assert(View::stack.back() == v);
    View::stack.pop_back();
    g_object_ref(v->panel);
    v->visible = false;
    gtk_container_remove(GTK_CONTAINER(panels), v->panel);
    
    auto next(View::stack.back());
    gtk_container_add(GTK_CONTAINER(panels), next->panel);
    gtk_widget_show_all(next->panel);
    if (View::stack.size() == 1) {
      gtk_widget_grab_focus(reader->entry);
    } else if (next->focused) {
      gtk_widget_grab_focus(next->focused);
      next->visible = true;
    }
  }

  void switch_view() {
    if (View::stack.size() > 2) {
      View *prev(View::stack.back());
      g_object_ref(prev->panel);
      gtk_container_remove(GTK_CONTAINER(panels), prev->panel);
      View::stack.pop_back();
      View *next(View::stack.back());
      View::stack.insert(std::next(View::stack.begin()), prev);
      gtk_container_add(GTK_CONTAINER(panels), next->panel);  
      gtk_widget_show_all(next->panel);
      gtk_widget_grab_focus(next->focused);
      next->visible = true;
    }
  }
}}

#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  std::stack<View *> View::stack;

  static void on_focus_out(gpointer *_, View *v) {
    log(v->ctx, "focus out");
  }
  
  View::View(Ctx &ctx, const str &lbl): 
    ctx(ctx), panel(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)), focused(panel) { 
    add_style(panel, "view");
    gtk_widget_set_margin_start(panel, 5);
    gtk_widget_set_margin_end(panel, 5);
    gtk_widget_set_margin_top(panel, 5);
    gtk_widget_set_margin_bottom(panel, 5);

    GtkWidget *l = gtk_label_new(lbl.c_str());
    add_style(l, "view_label");
    gtk_widget_set_halign(l, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), l);

    g_signal_connect(panel, "focus-out-event", G_CALLBACK(on_focus_out), this);
  }

  View::~View() {
    gtk_widget_destroy(panel);
  }
  
  GtkWidget *View::ptr() {
      return panel;
  }

  void push_view(View &v) {
    if (!View::stack.empty()) {
      View *prev(View::stack.top());
      prev->focused = gtk_window_get_focus(GTK_WINDOW(window));
      g_object_ref(prev->panel);
      gtk_container_remove(GTK_CONTAINER(gui::panels), prev->panel);
    }

    View::stack.push(&v);    
    gtk_container_add(GTK_CONTAINER(gui::panels), v.panel);  
    gtk_widget_show_all(v.ptr());
    gtk_widget_grab_focus(v.focused);
  }

  void pop_view(View &v) {
    assert(View::stack.top() == &v);
    View::stack.pop();
    g_object_ref(v.panel);
    gtk_container_remove(GTK_CONTAINER(gui::panels), v.panel);
    
    if (View::stack.empty()) {
      gtk_widget_grab_focus(reader->entry);
    } else {
      auto next(View::stack.top());
      gtk_container_add(GTK_CONTAINER(gui::panels), next->panel);
      gtk_widget_show_all(next->ptr());
      gtk_widget_grab_focus(next->focused);
    }

    delete &v;
  }
}}

#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  std::stack<View *> View::stack;

  View::View(Ctx &ctx, const str &lbl): 
    ctx(ctx), panel(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)) { 
    add_style(panel, "view");
    gtk_widget_set_margin_start(panel, 5);
    gtk_widget_set_margin_end(panel, 5);
    gtk_widget_set_margin_top(panel, 5);
    gtk_widget_set_margin_bottom(panel, 5);

    GtkWidget *l = gtk_label_new(lbl.c_str());
    add_style(l, "view_label");
    gtk_widget_set_halign(l, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), l);
  }

  View::~View() {
    gtk_widget_destroy(panel);
  }
  
  GtkWidget *View::ptr() {
      return panel;
  }

  void push_view(View &v) {
    if (!View::stack.empty()) {
      const View *prev(View::stack.top());
      g_object_ref(prev->panel);
      gtk_container_remove(GTK_CONTAINER(gui::panels), prev->panel);
    }

    View::stack.push(&v);    
    gtk_container_add(GTK_CONTAINER(gui::panels), v.panel);  
    gtk_widget_show_all(v.ptr());
    v.focus();
  }

  void pop_view(View &v) {
    assert(View::stack.top() == &v);
    View::stack.pop();
    g_object_ref(v.panel);
    gtk_container_remove(GTK_CONTAINER(gui::panels), v.panel);
    
    if (View::stack.empty()) {
      reader->focus();
    } else {
      auto next(View::stack.top());
      gtk_container_add(GTK_CONTAINER(gui::panels), next->panel);
      next->focus();
    }

    delete &v;
  }
}}

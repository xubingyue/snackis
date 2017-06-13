#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  std::stack<View *> View::stack;

  View::View(Ctx &ctx): 
    ctx(ctx), panel(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)) { 
    GtkWidget *lbl = gtk_label_new("Login");
    add_style(lbl, "view_label");
    gtk_widget_set_halign(lbl, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(panel), lbl);
  }

  GtkWidget *View::ptr() {
      return panel;
  }

  void View::push_view() {
    stack.push(this);
    gtk_container_add(GTK_CONTAINER(gui::panels), panel);  
    focus();
  }

  void View::pop_view() {
    assert(stack.top() == this);
    stack.pop();
    gtk_container_remove(GTK_CONTAINER(gui::panels), panel);
  }

}}

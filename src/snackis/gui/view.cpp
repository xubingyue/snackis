#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  std::stack<View *> View::stack;

  View::View(Ctx &ctx, GtkWidget *pan): ctx(ctx), panel(pan) 
  { }

  GtkWidget *View::ptr() {
      return panel;
  }

  void View::push_view() {
    stack.push(this);
    gtk_container_add(GTK_CONTAINER(gui::panels), panel);  
  }

  void View::pop_view() {
    assert(stack.top() == this);
    stack.pop();
    gtk_container_remove(GTK_CONTAINER(gui::panels), panel);
  }

}}

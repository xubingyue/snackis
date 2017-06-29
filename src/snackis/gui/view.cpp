#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  std::vector<View *> View::stack;

  View::View(Ctx &ctx, const str &lbl, const str &inf): 
    ctx(ctx),
    panel(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0)),
    label(gtk_label_new(lbl.c_str())),
    info(gtk_label_new(inf.c_str())),
    focused(panel),
    needs_init(true) { 
    add_style(panel, "view");
    gtk_widget_set_margin_start(panel, 5);
    gtk_widget_set_margin_end(panel, 5);
    gtk_widget_set_margin_top(panel, 5);
    gtk_widget_set_margin_bottom(panel, 5);

    auto hdr = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(panel), hdr);
    gtk_widget_set_halign(info, GTK_ALIGN_START);
    gtk_widget_set_hexpand(info, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(hdr), info);
    add_style(label, "view_label");
    gtk_widget_set_halign(label, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(hdr), label);
  }

  View::~View() {
    gtk_widget_destroy(panel);
  }
  
  GtkWidget *View::ptr() {
      return panel;
  }
  
  void push_view(View &v) {
    if (v.needs_init) { v.init(); }
	   
    if (!View::stack.empty()) {
      View *prev(View::stack.back());
      prev->focused = gtk_window_get_focus(GTK_WINDOW(window));
      g_object_ref(prev->panel);
      gtk_container_remove(GTK_CONTAINER(gui::panels), prev->panel);
    }

    View::stack.push_back(&v);    
    gtk_container_add(GTK_CONTAINER(gui::panels), v.panel);  
    gtk_widget_show_all(v.ptr());
    gtk_widget_grab_focus(v.focused);
  }

  void pop_view(View &v) {
    assert(View::stack.back() == &v);
    View::stack.pop_back();
    g_object_ref(v.panel);
    gtk_container_remove(GTK_CONTAINER(gui::panels), v.panel);
    
    if (View::stack.empty()) {
      gtk_widget_grab_focus(reader->entry);
    } else {
      auto next(View::stack.back());
      gtk_container_add(GTK_CONTAINER(gui::panels), next->panel);
      gtk_widget_show_all(next->panel);
      if (next->focused) {
	gtk_widget_grab_focus(next->focused);
      }
    }

    delete &v;
  }

  void swap_views() {
    if (View::stack.size() > 1) {
      View *prev(View::stack.back());
      g_object_ref(prev->panel);
      gtk_container_remove(GTK_CONTAINER(gui::panels), prev->panel);
      View::stack.pop_back();
      View *next(View::stack.back());
      View::stack.pop_back();
      View::stack.push_back(prev);
      View::stack.push_back(next);
      gtk_container_add(GTK_CONTAINER(gui::panels), next->panel);  
      gtk_widget_show_all(next->panel);
      gtk_widget_grab_focus(next->focused);
    }
  }
}}

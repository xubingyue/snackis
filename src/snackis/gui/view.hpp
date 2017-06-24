#ifndef SNACKIS_GUI_VIEW_HPP
#define SNACKIS_GUI_VIEW_HPP

#include <stack>
#include "snackis/gui/widget.hpp"

namespace snackis {
  struct Ctx;
namespace gui {
  struct View: Widget {
    static std::stack<View *> stack;
    Ctx &ctx;
    GtkWidget *panel, *label, *info, *focused;
    
    View(Ctx &ctx, const str &lbl, const str &inf="");
    virtual ~View();
    GtkWidget *ptr() override;
  };

  void push_view(View &v);
  void pop_view(View &v);
}}


#endif

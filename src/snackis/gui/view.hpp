#ifndef SNACKIS_GUI_VIEW_HPP
#define SNACKIS_GUI_VIEW_HPP

#include <stack>
#include "snackis/gui/widget.hpp"

namespace snackis {
  struct Ctx;
namespace gui {
  struct View: public Widget {
    static std::stack<View *> stack;
    Ctx &ctx;
    GtkWidget *panel;
    
    View(Ctx &ctx, const str &lbl);
    virtual ~View();
    void push_view();
    void pop_view();
    GtkWidget *ptr() override;
  };
}}


#endif

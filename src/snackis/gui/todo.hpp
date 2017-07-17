#ifndef SNACKIS_GUI_TODO_HPP
#define SNACKIS_GUI_TODO_HPP

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  const int TODO_MAX(21);

  struct Todo: View {
    GtkListStore *store;
    GtkWidget *lst, *cancel_btn;

    Todo(Ctx &ctx);
    void load() override;
  };
}}

#endif

#ifndef SNACKIS_GUI_UNDO_HPP
#define SNACKIS_GUI_UNDO_HPP

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Undo: View {
    GtkListStore *store;
    GtkWidget *lst, *revert_btn, *cancel_btn;

    Undo(Ctx &ctx);
  };
}}

#endif

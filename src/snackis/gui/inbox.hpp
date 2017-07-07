#ifndef SNACKIS_GUI_INBOX_HPP
#define SNACKIS_GUI_INBOX_HPP

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Inbox: View {
    GtkListStore *msgs;
    GtkWidget *lst, *close_btn;

    Inbox(Ctx &ctx);
  };
}}

#endif

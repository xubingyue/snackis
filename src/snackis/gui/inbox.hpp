#ifndef SNACKIS_GUI_INBOX_HPP
#define SNACKIS_GUI_INBOX_HPP

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Inbox: View {
    GtkListStore *store;
    GtkWidget *lst, *cancel_btn;

    Inbox(Ctx &ctx);
    void load() override;
  };
}}

#endif

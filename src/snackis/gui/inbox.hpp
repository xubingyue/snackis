#ifndef SNACKIS_GUI_INBOX_HPP
#define SNACKIS_GUI_INBOX_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Inbox: View {
    GtkWidget *list, *accept, *reject, *close;
    db::Rec<Msg> *sel;
    Inbox(Ctx &ctx);
  };
}}

#endif

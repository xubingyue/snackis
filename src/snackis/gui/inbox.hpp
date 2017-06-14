#ifndef SNACKIS_GUI_INBOX_HPP
#define SNACKIS_GUI_INBOX_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Inbox: public View {
    GtkWidget *list, *close;
    
    Inbox(Ctx &ctx);
    void focus() override;
  };
}}

#endif

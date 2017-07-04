#ifndef SNACKIS_GUI_SETUP_HPP
#define SNACKIS_GUI_SETUP_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Setup: View {
    GtkWidget *name, *email,
      *load_folder, *save_folder,
      *imap_url, *imap_port, *imap_user, *imap_pass, *imap_poll,
      *smtp_url, *smtp_port, *smtp_user, *smtp_pass, *smtp_poll,
      *save, *cancel;
    
    Setup(Ctx &ctx);
  };
}}

#endif

#ifndef SNACKIS_GUI_SETUP_HPP
#define SNACKIS_GUI_SETUP_HPP

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Setup;
  
  struct Server {
    GtkWidget *box, *url, *port, *user, *pass, *poll;
    Server(Setup &v, GCallback fn);
  };

  struct Setup: View {
    GtkWidget *name, *email,
      *load_folder, *save_folder,
      *save, *cancel;
    Server imap, smtp;
    
    Setup(Ctx &ctx);
    void load() override;
  };
}}

#endif

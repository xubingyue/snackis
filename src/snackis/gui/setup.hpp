#ifndef SNACKIS_GUI_SETUP_HPP
#define SNACKIS_GUI_SETUP_HPP

#include "snackis/gui/view.hpp"

namespace snackis {
  struct ServerSettings;
  
namespace gui {
  struct Setup;
  
  struct Server {
    Ctx &ctx;
    ServerSettings &settings;
    GtkWidget *box, *url, *port, *user, *pass, *poll;
    Server(Ctx &ctx, ServerSettings &sts, GCallback fn);
  };

  struct Setup: View {
    GtkWidget *name, *email,
      *pass, *pass_repeat,
      *load_folder, *save_folder,
      *save, *cancel;
    Server imap, smtp;
    
    Setup(Ctx &ctx);
    void load() override;
  };
}}

#endif

#ifndef SNACKIS_SETTINGS_HPP
#define SNACKIS_SETTINGS_HPP

#include "snackis/setting.hpp"

namespace snackis {
  struct Settings {
    Setting me, editor, priv_key,
      imap_server, imap_port, imap_user, imap_pass,
      smtp_server, smtp_port, smtp_user, smtp_pass;

    Settings(Ctx &ctx);
  };
}

#endif

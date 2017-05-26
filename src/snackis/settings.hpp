#ifndef SNACKIS_SETTINGS_HPP
#define SNACKIS_SETTINGS_HPP

#include "snackis/setting.hpp"

namespace snackis {
  struct Settings {
    Setting<void *> me;
    Setting<str> editor;
    Setting<crypt::Key> priv_key;
    
    Setting<str> imap_server;
    Setting<int64_t> imap_port;
    Setting<str> imap_user, imap_pass;

    Setting<str> smtp_server;
    Setting<int64_t> smtp_port;
    Setting<str> smtp_user, smtp_pass;

    Settings(Ctx &ctx);
  };
}

#endif

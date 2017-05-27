#ifndef SNACKIS_SETTINGS_HPP
#define SNACKIS_SETTINGS_HPP

#include "snackis/setting.hpp"

namespace snackis {
  struct Settings {
    Setting<Peer *> whoami;
    Setting<crypt::Key> crypt_key;
    Setting<str> editor;
    
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

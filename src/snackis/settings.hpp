#ifndef SNACKIS_SETTINGS_HPP
#define SNACKIS_SETTINGS_HPP

#include "snackis/setting.hpp"

namespace snackis {
  struct Settings {
    Setting<UId> whoami;
    Setting<crypt::Key> crypt_key;

    Setting<str> load_folder, save_folder;
    
    Setting<str> imap_url;
    Setting<int64_t> imap_port;
    Setting<str> imap_user, imap_pass;
    Setting<int64_t> imap_poll;
    
    Setting<str> smtp_url;
    Setting<int64_t> smtp_port;
    Setting<str> smtp_user, smtp_pass;
    Setting<int64_t> smtp_poll;
    
    Settings(Ctx &ctx);
  };
}

#endif

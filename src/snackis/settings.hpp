#ifndef SNACKIS_SETTINGS_HPP
#define SNACKIS_SETTINGS_HPP

#include "snackis/setting.hpp"

namespace snackis {
  struct ServerSettings {
    Setting<str> url;
    Setting<int64_t> port;
    Setting<str> user, pass;
    Setting<int64_t> poll;
    
    ServerSettings(Ctx &ctx, const str &n, int64_t port);
  };

  struct Settings {
    Setting<UId> whoami;
    Setting<crypt::Key> crypt_key;
    Setting<str> load_folder, save_folder;
    ServerSettings imap, smtp;
    
    Settings(Ctx &ctx);
  };
}

#endif

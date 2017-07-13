#include "snackis/ctx.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/key_type.hpp"

namespace snackis {
  ServerSettings::ServerSettings(Ctx &ctx, const str &n, int64_t port):
    url(ctx,  fmt("%0_url", n),  str_type,   fmt("%0.gmail.com", n)),
    port(ctx, fmt("%0_port", n), int64_type, port),
    user(ctx, fmt("%0_user", n), str_type,   str("")),
    pass(ctx, fmt("%0_pass", n), str_type,   str("")),
    poll(ctx, fmt("%0_poll", n), int64_type, 0)
  { }
  
  Settings::Settings(Ctx &ctx):
    whoami(ctx,    "whoami",    uid_type),
    crypt_key(ctx, "crypt_key", crypt::key_type),

    load_folder(ctx, "load_folder", str_type, str("load/")),
    save_folder(ctx, "save_folder", str_type, str("save/")),
    imap(ctx, "imap", 993),
    smtp(ctx, "smtp", 587)
  { }
}

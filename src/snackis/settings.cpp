#include "snackis/ctx.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/key_type.hpp"

namespace snackis {
  Settings::Settings(Ctx &ctx):
    whoami(ctx,    "whoami",    uid_type),
    crypt_key(ctx, "crypt_key", crypt::key_type),

    load_folder(ctx, "load_folder", str_type, str("load/")),
    save_folder(ctx, "save_folder", str_type, str("save/")),
    
    imap_url(ctx,  "imap_url",  str_type,   str("imap.gmail.com")),
    imap_port(ctx, "imap_port", int64_type, 993),
    imap_user(ctx, "imap_user", str_type,   str("")),
    imap_pass(ctx, "imap_pass", str_type,   str("")),
    imap_poll(ctx, "imap_poll", int64_type, 0),
    
    smtp_url(ctx,  "smtp_url",  str_type,   str("smtp.gmail.com")),
    smtp_port(ctx, "smtp_port", int64_type, 587),
    smtp_user(ctx, "smtp_user", str_type,   str("")),
    smtp_pass(ctx, "smtp_pass", str_type,   str("")),
    smtp_poll(ctx, "smtp_poll", int64_type, 0)
  { }
}

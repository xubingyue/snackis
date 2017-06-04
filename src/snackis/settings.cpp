#include "snackis/ctx.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/crypt/key_type.hpp"

namespace snackis {
  Settings::Settings(Ctx &ctx):
    whoami(ctx,    "whoami",    ctx.db.peers.rec_type, nullopt),
    crypt_key(ctx, "crypt_key", crypt::key_type,       nullopt),
    editor(ctx,    "editor",    str_type,              str("nano")),
    
    imap_server(ctx, "imap_server", str_type,   str("imap.gmail.com")),
    imap_port(ctx,   "imap_port",   int64_type, 993),
    imap_user(ctx,   "imap_user",   str_type,   nullopt),
    imap_pass(ctx,   "imap_pass",   str_type,   nullopt),
    
    smtp_server(ctx, "smtp_server", str_type,   str("smtp.gmail.com")),
    smtp_port(ctx,   "smtp_port",   int64_type, 587),
    smtp_user(ctx,   "smtp_user",   str_type,   nullopt),
    smtp_pass(ctx,   "smtp_pass",   str_type,   nullopt) 
  { }
}

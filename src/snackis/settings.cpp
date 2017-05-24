#include "snackis/ctx.hpp"
#include "snackis/settings.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/crypt/key_type.hpp"

namespace snackis {
  Settings::Settings(Ctx &ctx):
    me(ctx,       "me",       ctx.db.peers.rec_type, none),
    editor(ctx,   "editor",   str_type,              Val(str("nano"))),
    priv_key(ctx, "priv_key", crypt::key_type,       none),
    
    imap_server(ctx, "imap_server", str_type, Val(str("imap.gmail.com"))),
    imap_port(ctx,   "imap_port", int64_type, Val(int64_t(993))),
    imap_user(ctx,   "imap_user", str_type,   none),
    imap_pass(ctx,   "imap_pass", str_type,   none),
    
    smtp_server(ctx, "smtp_server", str_type,   Val(str("smtp.gmail.com"))),
    smtp_port(ctx,   "smtp_port",   int64_type, Val(int64_t(587))),
    smtp_user(ctx,   "smtp_user",   str_type,   none),
    smtp_pass(ctx,   "smtp_pass",   str_type,   none) 
  { }
}

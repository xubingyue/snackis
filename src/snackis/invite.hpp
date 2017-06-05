#ifndef SNACKIS_INVITE_HPP
#define SNACKIS_INVITE_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {
  struct Invite: public Rec {
    str to;
    Time sent_at, accept_at, reject_at;
    
    Invite(Ctx &ctx, const str &to);
    Invite(const db::Table<Invite> &tbl, const db::Rec<Invite> &rec);
  };

  void send(Invite &inv);
}

#endif

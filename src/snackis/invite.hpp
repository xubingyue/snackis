#ifndef SNACKIS_INVITE_HPP
#define SNACKIS_INVITE_HPP

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"
#include "snackis/db/rec.hpp"

namespace snackis {
  struct Msg;
  struct Peer;
  
  struct Invite: Rec {
    str to;
    Time posted_at;
    
    Invite(Ctx &ctx, const str &to);
    Invite(const db::Table<Invite> &tbl, const db::Rec<Invite> &rec);
  };

  void post_msg(Invite &inv);
  Peer accept_invite(const Msg &in);
  Peer invite_accepted(const Msg &in);
  void reject_invite(const Msg &in);
  void invite_rejected(const Msg &in);
}

#endif

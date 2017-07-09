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
    Invite(Ctx &ctx, const db::Rec<Invite> &src);
  };

  void send(Invite &inv);
  void send_accept(const Msg &in);
  void invite_accepted(const Msg &in);
  void reject_invite(const Msg &in);
  void invite_rejected(const Msg &in);
}

#endif

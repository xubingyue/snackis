#ifndef SNACKIS_SCRIPT_HPP
#define SNACKIS_SCRIPT_HPP

#include <set>

#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/db/key.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec_type.hpp"

namespace snackis {
  struct Msg;
  
  struct Script: IdRec {   
    UId owner_id;
    Time created_at, changed_at;
    str name;
    std::set<str> tags;
    str code;
    std::set<UId> peer_ids;
    
    Script(Ctx &ctx, UId id=UId(true));
    Script(Ctx &ctx, const db::Rec<Script> &rec);   
    Script(const Msg &msg);
  };

  extern db::Col<Script, UId>           script_id;
  extern db::Col<Script, UId>           script_owner_id;
  extern db::Col<Script, Time>          script_created_at, script_changed_at;
  extern db::Col<Script, str>           script_name;
  extern db::Col<Script, std::set<str>> script_tags;
  extern db::Col<Script, str>           script_code;
  extern db::Col<Script, std::set<UId>> script_peer_ids;

  extern db::Key<Script, UId> script_key;
  extern db::Schema<Script> script_cols;
  extern db::RecType<Script> script_type;
  
  void copy(Script &fd, const Msg &msg);
  opt<Script> find_script_id(Ctx &ctx, UId id);
  Script get_script_id(Ctx &ctx, UId id);
  Feed get_feed(const Script &sct);
  void send(const Script &sct, const Peer &pr);
  void send(const Script &sct);
}

#endif

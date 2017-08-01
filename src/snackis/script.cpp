#include "snackis/ctx.hpp"
#include "snackis/script.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"

namespace snackis {
  db::Col<Script, UId> script_id("id", uid_type, &Script::id);
  db::Col<Script, UId> script_owner_id("owner_id", uid_type, &Script::owner_id);
  db::Col<Script, Time> script_created_at("created_at",
					  time_type,
					  &Script::created_at);
  db::Col<Script, Time> script_changed_at("changed_at",
					  time_type,
					  &Script::changed_at);
  db::Col<Script, std::set<str>> script_tags("tags", str_set_type, &Script::tags);
  db::Col<Script, str> script_code("code", str_type, &Script::code);
  db::Col<Script, std::set<UId>> script_peer_ids("peer_ids",
					     uid_set_type,
					     &Script::peer_ids);

  db::Key<Script, UId> script_key(script_id);
  
  db::Schema<Script> script_cols({&script_id, &script_owner_id, &script_created_at,
	&script_changed_at, &script_tags, &script_code, &script_peer_ids});

  db::RecType<Script> script_type(script_cols);

  Script::Script(Ctx &ctx, UId id):
    IdRec(ctx, id),
    owner_id(whoamid(ctx)),
    created_at(now()),
    changed_at(created_at)
  { }

  Script::Script(Ctx &ctx, const db::Rec<Script> &src): IdRec(ctx, null_uid) {
    db::copy(*this, src);
  }
  
  Script::Script(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.script, script_id)),
    owner_id(msg.from_id)
  {
    copy(*this, msg);
  }

  void copy(Script &fd, const Msg &msg) {
    Ctx &ctx(fd.ctx);
    db::copy(ctx.db.scripts_share, fd, msg.script);
    fd.peer_ids.erase(whoamid(ctx));
    fd.peer_ids.insert(msg.from_id);
  }

  opt<Script> find_script_id(Ctx &ctx, UId id) {
    auto fnd(db::find(ctx.db.scripts, id));
    if (!fnd) { return nullopt; }
    return Script(ctx, *fnd);
  }

  Script get_script_id(Ctx &ctx, UId id) {
    auto found(find_script_id(ctx, id));
    CHECK(found, _);
    return *found;
  }

  void send(const Script &sct, const Peer &pr) {
    Ctx &ctx(sct.ctx);
    Msg msg(ctx, Msg::SCRIPT);
    msg.to = pr.email;
    msg.to_id = pr.id;
    db::copy(ctx.db.scripts_share, msg.script, sct);
    insert(ctx.db.outbox, msg);
  }

  void send(const Script &sct) {
    Ctx &ctx(sct.ctx);
    for (auto &pid: sct.peer_ids) {
      auto pr(find_peer_id(ctx, pid));
      if (pr) { send(sct, *pr); }
    }
  }
}

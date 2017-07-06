#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/project.hpp"

namespace snackis {
  Project::Project(Ctx &ctx):
    IdRec(ctx),
    owner_id(whoami(ctx).id),
    created_at(now()),
    changed_at(created_at),
    active(true)
  { }

  Project::Project(Ctx &ctx, const db::Rec<Project> &rec): IdRec(ctx, null_uid) {
    copy(*this, rec);
  }

  Project::Project(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.project, msg.ctx.db.project_id))
  {
    copy(*this, msg);
  }

  void copy(Project &dst, const Msg &src) {
    db::copy(dst, src.project);    
    dst.peer_ids.insert(src.from_id);    
    dst.peer_ids.erase(whoami(src.ctx).id);
  }

  opt<Project> find_project_id(Ctx &ctx, UId id) {
    db::Rec<Project> rec;
    set(rec, ctx.db.project_id, id);
    if (!load(ctx.db.projects, rec)) { return nullopt; }
    return Project(ctx, rec);
  }

  Project get_project_id(Ctx &ctx, UId id) {
    auto found(find_project_id(ctx, id));
    
    if (!found) {
      ERROR(Db, fmt("Project id not found: %0", id));
    }

    return *found;
  }

  Feed get_feed(const Project &prj) {
    Ctx &ctx(prj.ctx);
    db::Rec<Feed> rec;
    db::set(rec, ctx.db.feed_id, prj.id);
    Feed feed(ctx, rec);

    if (!db::load(ctx.db.feeds, feed)) {
      db::Trans trans(ctx);
      TRY(try_create);
      feed.name = fmt("Project %0", id_str(prj));
      feed.visible = false;
      feed.peer_ids = prj.peer_ids;
      db::insert(ctx.db.feeds, feed);
      if (try_create.errors.empty()) { db::commit(trans); }
    }
    
    return feed;
  }
}

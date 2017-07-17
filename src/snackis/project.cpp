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
    db::copy(*this, rec);
  }

  Project::Project(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.project, msg.ctx.db.project_id)),
    owner_id(msg.from_id),
    created_at(now()),
    changed_at(created_at)
  {
    db::copy(*this, msg.project);
    peer_ids.insert(msg.from_id);
  }

  opt<Project> find_project_id(Ctx &ctx, UId id) {
    db::Rec<Project> rec;
    set(rec, ctx.db.project_id, id);
    if (!load(ctx.db.projects, rec)) { return nullopt; }
    return Project(ctx, rec);
  }

  Project get_project_id(Ctx &ctx, UId id) {
    auto found(find_project_id(ctx, id));
    CHECK(found, _);
    return *found;
  }

  Feed get_feed(const Project &prj) {
    Ctx &ctx(prj.ctx);
    db::Rec<Feed> rec;
    db::set(rec, ctx.db.feed_id, prj.id);
    Feed fd(ctx, rec);

    if (!db::load(ctx.db.feeds, fd)) {
      db::Trans trans(ctx);
      TRY(try_create);
      fd.name = fmt("Project %0", id_str(prj));
      fd.tags = prj.tags;
      fd.owner_id = prj.owner_id;
      fd.active = true;
      fd.visible = false;
      fd.peer_ids = prj.peer_ids;
      db::insert(ctx.db.feeds, fd);

      if (try_create.errors.empty()) {
	db::commit(trans, fmt("Created Project Feed %0", id_str(fd)));
      }
    }
    
    return fd;
  }
}

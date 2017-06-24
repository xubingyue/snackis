#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/project.hpp"

namespace snackis {
  Project::Project(Ctx &ctx): Rec(ctx), owner_id(whoami(ctx).id)
  { }

  Project::Project(Ctx &ctx, const db::Rec<Project> &rec): Rec(ctx), id(false) {
    copy(*this, rec);
  }

  Project::Project(const Msg &msg):
    Rec(msg.ctx),
    //id(msg.project_id),
    owner_id(msg.from_id)
    //name(msg.project_name),
    //info(msg.project_info)
  { }

  opt<Project> find_project_id(Ctx &ctx, UId id) {
    db::Rec<Project> rec;
    set(rec, ctx.db.project_id, id);
    if (!load(ctx.db.projects, rec)) { return nullopt; }
    return Project(ctx, rec);
  }
}

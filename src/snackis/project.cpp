#include <iostream>
#include "snackis/ctx.hpp"
#include "snackis/peer.hpp"
#include "snackis/project.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"

namespace snackis {
  db::Col<Project, UId> project_id("id", uid_type, &Project::id);
  db::Col<Project, UId> project_owner_id("owner_id", uid_type, &Project::owner_id);
  db::Col<Project, Time> project_created_at("created_at",
					    time_type,
					    &Project::created_at);
  db::Col<Project, Time> project_changed_at("changed_at",
					    time_type,
					    &Project::changed_at);
  db::Col<Project, str> project_name("name", str_type, &Project::name);
  db::Col<Project, str> project_info("info", str_type, &Project::info);
  db::Col<Project, std::set<str>> project_tags("tags", str_set_type, &Project::tags);
  db::Col<Project, bool> project_active("active", bool_type, &Project::active);
  db::Col<Project, std::set<UId>> project_peer_ids("peer_ids",
						   uid_set_type,
						   &Project::peer_ids);

  db::Schema<Project> project_key({&project_id});
  
  db::Schema<Project> project_cols({&project_id, &project_owner_id, 
	&project_created_at, &project_changed_at, &project_name, &project_info, 
	&project_tags, &project_active, &project_peer_ids});

  db::RecType<Project> project_type(project_cols);

  Project::Project(Ctx &ctx):
    IdRec(ctx),
    owner_id(whoamid(ctx)),
    created_at(now()),
    changed_at(created_at),
    active(true)
  { }

  Project::Project(Ctx &ctx, const db::Rec<Project> &rec): IdRec(ctx, null_uid) {
    db::copy(*this, rec);
  }

  Project::Project(const Msg &msg):
    IdRec(msg.ctx, *db::get(msg.project, project_id)),
    owner_id(msg.from_id),
    created_at(now()),
    changed_at(created_at)
  {
    copy(*this, msg);
  }

  void copy(Project &prj, const Msg &msg) {
    Ctx &ctx(prj.ctx);
    db::copy(ctx.db.projects_share, prj, msg.project);
    prj.peer_ids.erase(whoamid(ctx));
    prj.peer_ids.insert(msg.from_id);
  }
  
  opt<Project> find_project_id(Ctx &ctx, UId id) {
    db::Rec<Project> rec;
    set(rec, project_id, id);
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
    db::set(rec, feed_id, prj.id);
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

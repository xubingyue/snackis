#ifndef SNACKIS_PROJECT_HPP
#define SNACKIS_PROJECT_HPP

#include <set>

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"
#include "snackis/db/schema.hpp"
#include "snackis/db/rec_type.hpp"

namespace snackis {
  struct Feed;
  struct Msg;
  
  struct Project: IdRec {
    UId owner_id;
    Time created_at, changed_at;
    str name, info;
    std::set<str> tags;
    bool active;
    std::set<UId> peer_ids;
    
    Project(Ctx &ctx);
    Project(Ctx &ctx, const db::Rec<Project> &rec);   
    Project(const Msg &msg);
  };

  extern db::Col<Project, UId>           project_id;
  extern db::Col<Project, UId>           project_owner_id;
  extern db::Col<Project, Time>          project_created_at, project_changed_at;
  extern db::Col<Project, str>           project_name, project_info;
  extern db::Col<Project, std::set<str>> project_tags;
  extern db::Col<Project, bool>          project_active;
  extern db::Col<Project, std::set<UId>> project_peer_ids;

  extern db::Schema<Project> project_key, project_cols;
  extern db::RecType<Project> project_type;

  void copy(Project &prj, const Msg &msg);
  opt<Project> find_project_id(Ctx &ctx, UId id);
  Project get_project_id(Ctx &ctx, UId id);
  Feed get_feed(const Project &prj);
}

#endif

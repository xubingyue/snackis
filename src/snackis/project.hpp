#ifndef SNACKIS_PROJECT_HPP
#define SNACKIS_PROJECT_HPP

#include <set>

#include "snackis/id_rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Feed;
  struct Msg;
  
  struct Project: IdRec {
    UId owner_id;
    Time created_at, changed_at;
    str name, info;
    bool active;
    std::set<UId> peer_ids;
    
    Project(Ctx &ctx);
    Project(Ctx &ctx, const db::Rec<Project> &rec);   
    Project(const Msg &msg);
  };

  opt<Project> find_project_id(Ctx &ctx, UId id);
  Project get_project_id(Ctx &ctx, UId id);
  Feed get_feed(const Project &prj);
}

#endif

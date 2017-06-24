#ifndef SNACKIS_PROJECT_HPP
#define SNACKIS_PROJECT_HPP

#include <set>

#include "snackis/rec.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Project: Rec {
    UId id;
    str name;
    UId owner_id;
    str info;
    std::set<UId> peer_ids;
    
    Project(Ctx &ctx);
    Project(Ctx &ctx, const db::Rec<Project> &rec);   
    Project(const Msg &msg);
  };

  opt<Project> find_project_id(Ctx &ctx, UId id);
}

#endif

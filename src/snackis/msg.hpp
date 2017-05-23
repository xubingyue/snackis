#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include <string>

#include "snackis/rec.hpp"
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Msg: public Rec {
    UId id;
    std::string type;
    int64_t proto_rev;
    std::string sent_by, sent_to;
    Time fetched_at;

    Msg(Ctx &ctx, const std::string &type);
    Msg(Ctx &ctx);
  };
}

#endif

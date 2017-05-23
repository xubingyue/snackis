#ifndef SNACKIS_MSG_HPP
#define SNACKIS_MSG_HPP

#include <string>
#include "snackis/core/time.hpp"
#include "snackis/core/uid.hpp"

namespace snackis {
  struct Msg {
    UId id;
    std::string type;
    int64_t proto_rev;
    std::string sent_by, sent_to;
    Time fetched_at;

    Msg(const std::string &type);
    Msg();
  };
}

#endif

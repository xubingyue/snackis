#ifndef SNACKIS_DB_LOOP_HPP
#define SNACKIS_DB_LOOP_HPP

#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  struct Loop {
    Chan<Msg> inbox;
    Loop(size_t max_buf);
  };
}}

#endif
